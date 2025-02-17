/**
 * This file contains the implementation of the second pass of a two-pass assembly processing for a source file.
 * It includes functions to parse and process each line of the source file during the second pass,
 * as well as functions to encode operands, symbols, and operations into the assembly code.
 */

#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "utils.h"
#include "globals.h"
#include "symbol_structs.h"

/* Definition of a symbol in the symbol table (linked list) */
struct symbol {
    char name[MAX_SYMBOL_LEN]; /* The name of the symbol */
    unsigned int address; /* The address associated with the symbol */
    statement_type type; /* The type of statement the symbol belongs to */
    boolean is_ext; /* Indicates if the symbol is an external symbol */
    boolean is_ent; /* Indicates if the symbol is an entry symbol */
    Symbolptr next; /* Pointer to the next symbol in the symbol table */
};

/**
 * Performs the second pass of a two-pass processing on a source file.
 *
 * @param source_filename The name of the source file to process.
 *
 * @return A boolean indicating whether there were any errors during processing.
 *
 * @remarks This function relies on the global variables ic and line_num for processing.
 */
boolean second_process(char *source_filename) {
    char *modified_filename_macro;
    char line[MAX_LINE_LEN]; /* Buffer to store each line of the source file */
    FILE *extended_src_fd;
    boolean was_error;

    ic = 0;
    line_num = 1;
    ext_table = NULL;

    was_error = FALSE; /* Flag to track if there were any errors during processing */

    /* Generate a modified filename with a macro extension */
    modified_filename_macro = generate_new_filename(source_filename, FILE_MACRO);

    extended_src_fd = fopen(modified_filename_macro, "r");
    if (extended_src_fd == NULL) {
        print_error(CANNOT_OPEN_FILE);
        free(modified_filename_macro);
        return TRUE;
    }

    /* Read each line from the extended source file and process it */
    while (fgets(line, MAX_LINE_LEN, extended_src_fd)) {
        /* Trim leading and trailing whitespaces from the line before processing */
        trim_whitespaces(line);
        /* Check if the line should be ignored */
        if (!should_ignore(line)) {
            /* Parse and process the line in the second pass */
            if (!parse_line_second_pass(line)) {
                was_error = TRUE;
            }
        }

        line_num++;
    }

    free(modified_filename_macro);

    fclose(extended_src_fd);

    return was_error;
}

/**
 * Parses and processes a line during the second pass of assembly processing.
 *
 * @param line The line to parse and process.
 *
 * @return A boolean indicating whether the line was successfully parsed and processed.
 */
boolean parse_line_second_pass(char *line) {
    opcode op_val = NONE_OP;
    directive dir_val = NONE_DIR;
    char current_token[MAX_LINE_LEN];

    /* Extract the next token from the line, which could be a symbol or an operation/directive */
    copy_next_token(current_token, line, ":\t ");

    /* If the token is a symbol, skip it */
    if (is_symbol(current_token, TRUE)) {
        line = extract_remaining_seq(line, ":");
        copy_next_token(current_token, line, ",\t ");
    }

    /* Check if the token corresponds to an operation */
    if ((op_val = find_operation(current_token)) != NONE_OP) {
        line = extract_remaining_seq(line, ",\t ");

        /* If the second pass processing of the operation fails, return false */
        return process_operation_second_pass(op_val, line);
    /* Check if the token corresponds to a directive */
    } else if ((dir_val = find_directive(current_token)) != NONE_DIR) {
        line = extract_remaining_seq(line, ",\t ");
        if (dir_val == ENTRY) {
            /* Extract the symbol token after the ENTRY directive and make it an entry symbol */
            copy_next_token(current_token, line, "\t ");
            return make_entry(symbol_table, current_token);
        }
    }

    return TRUE;
}

/**
 * Processes an operation during the second pass of assembly processing.
 *
 * @param op_type   The opcode representing the operation to process.
 * @param line      The line containing the operation and its operands.
 *
 * @return A boolean indicating whether the operation was successfully processed.
 */
boolean process_operation_second_pass(opcode op_type, char *line) {
    boolean has_src = FALSE, has_dest = FALSE;
    addressing_mode src_addr_mode = NONE_ADDR, dest_addr_mode = NONE_ADDR;
    char src[MAX_OPERAND_LEN];
    char dest[MAX_OPERAND_LEN];

    /* Determine if the operation has source and destination operands */
    determine_operand(op_type, &has_src, &has_dest);

    /* Extract addressing mode for the source operand if it exists */
    if (has_src) {
        src_addr_mode = (addressing_mode)extract_bits(code[ic], SRC_MODE_START_POS, SRC_MODE_END_POS);
    }

    /* Extract addressing mode for the destination operand if it exists */
    if (has_dest) {
        dest_addr_mode = (addressing_mode)extract_bits(code[ic], DEST_MODE_START_POS, DEST_MODE_END_POS);
    }

    /* Extract source and destination operands from the line */
    if (has_dest) {
        if (has_src) {
            copy_next_token(src, line, ",\t ");
            line = extract_remaining_seq(line, ",\t ");
            line++; /* Skip comma */
            copy_next_token(dest, line, "\t ");
        } else {
            copy_next_token(dest, line, "\t ");
        }
    }

    ic++;

    /* Encode additional words based on the extracted operands and addressing modes */
    return encode_additional_words(src, dest, has_src, has_dest, src_addr_mode, dest_addr_mode);
}

/**
 * Determines the presence of source and destination operands for the given opcode.
 *
 * @param op_type   The opcode for which to determine the operands.
 * @param has_src   Pointer to a boolean indicating if the opcode has a source operand.
 * @param has_dest  Pointer to a boolean indicating if the opcode has a destination operand.
 */
void determine_operand(opcode op_type, boolean *has_src, boolean *has_dest) {
    switch (op_type)
    {
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP:
            *has_src = TRUE;
            *has_dest = TRUE;
            break;

        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case RED_OP:
        case PRN_OP:
        case JSR_OP:
            *has_src = FALSE;
            *has_dest = TRUE;
            break;

        case RTS_OP:
        case STOP_OP:
            *has_src = FALSE;
            *has_dest = FALSE;

        default:
            break;
    }
}

/**
 * Encodes additional words based on the source and destination operands and their addressing modes.
 *
 * @param src       The source operand.
 * @param dest      The destination operand.
 * @param has_src   Indicates whether the opcode has a source operand.
 * @param has_dest  Indicates whether the opcode has a destination operand.
 * @param src_mode  The addressing mode of the source operand.
 * @param dest_mode The addressing mode of the destination operand.
 *
 * @return A boolean indicating whether the encoding of additional words was successful.
 */
boolean encode_additional_words(char *src, char *dest, boolean has_src, boolean has_dest, addressing_mode src_mode, addressing_mode dest_mode) {
    boolean src_success = FALSE, dest_success = FALSE;

    if (has_src && has_dest && src_mode == REG_DIRECT_ADDR && dest_mode == REG_DIRECT_ADDR) {
        /* If both source and destination are registers, encode their values as a single word */
        append_word_to_code(encode_reg(src, FALSE) | encode_reg(dest, TRUE));
        return TRUE;
    }

    if (has_dest) {
        if (has_src) {
            /* Encode the source operand to code based on its addressing mode */
            src_success = encode_operand_to_code(src, src_mode, FALSE);
            /* Encode the destination operand to code based on its addressing mode */
            dest_success = encode_operand_to_code(dest, dest_mode, TRUE);
            return dest_success && src_success;
        }
        /* Encode the destination operand to code based on its addressing mode */
        dest_success = encode_operand_to_code(dest, dest_mode, TRUE);
        return dest_success;
    }

    return TRUE;
}

/**
 * Encodes a register value into a word for use in the assembly code.
 *
 * @param reg       The register value to encode.
 * @param is_dest   Indicates whether the register is used as a destination operand.
 *
 * @return The encoded word representing the register value.
 */
unsigned int encode_reg(char *reg, boolean is_dest) {
    unsigned int word = 0;
    int register_num = atoi(reg + SKIP_TO_NUM_REG); /* Skip '@' and 'r' characters to extract the register number */

    if (!is_dest) {
        /* Shift the register number to the left by the number of bits in a register */
        word = register_num << BITS_IN_REG;
    } else {
        /* Use the register number as is for the destination operand */
        word = register_num;
    }

    /* Encode the word with the appropriate ARE (Absolute) */
    word = encode_are(word, ABSOLUTE);
    return word;
}

/**
 * Encodes an operand into the assembly code based on its addressing mode.
 *
 * @param operand   The operand to encode.
 * @param addr_mode The addressing mode of the operand.
 * @param is_dest   A boolean indicating whether the operand is a destination operand.
 *
 * @return A boolean indicating whether the encoding of the operand was successful.
 */
boolean encode_operand_to_code(char *operand, addressing_mode addr_mode, boolean is_dest) {
    unsigned int word = 0;

    switch (addr_mode) {
        case IMMEDIATE_ADDR:
            /* Convert the operand to an integer and encode it as an absolute value */
            word = atoi(operand);
            word = encode_are(word, ABSOLUTE);
            append_word_to_code(word);
            return TRUE;

        case DIRECT_ADDR:
            /* Encode the operand as a symbol reference */
            return encode_symbol(operand);

        case REG_DIRECT_ADDR:
            /* Encode the operand as a register value */
            word = encode_reg(operand, is_dest);
            append_word_to_code(word);
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

/**
 * Encodes a symbol and appends the encoded value to the code segment.
 *
 * @param symbol_name The name of the symbol to encode.
 *
 * @return A boolean indicating whether the encoding of the symbol was successful.
 *
 * @remarks This function utilizes the global variables 'symbol_table', 'ext_table', and 'ic'.
 */
boolean encode_symbol(char *symbol_name) {
    unsigned int word = 0;

    if (is_existing_symbol(symbol_table, symbol_name)) {
        /* Retrieve the address of the symbol from the symbol table */
        word = get_symbol_addr(symbol_table, symbol_name);

        if (is_extern_symbol(symbol_table, symbol_name)) {
            /* Add the symbol to the external symbols table */
            add_ext_to_list(&ext_table, symbol_name, ic + MEM_START);
            word = encode_are(word, EXTERNAL);
        } else {
            /* If the symbol is not an external symbol, encode as a relocatable reference */
            word = encode_are(word, RELOCATABLE);
        }

        /* Append the encoded symbol address to the code */
        append_word_to_code(word);
    } else {
        ic++; /* Increase the instruction counter as a placeholder */
        print_error(SYMBOL_NOT_FOUND);
        return FALSE;
    }

    return TRUE;
}