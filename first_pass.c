/**
 * This file contains functions that perform the first pass of the assembly process for an assembler program.
 * The first pass is responsible for processing the source file, identifying symbols, operations, and directives,
 * and building the symbol table and code segments.
 */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "first_pass.h"
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
 * Processes the source file and performs the first pass of the assembly process.
 *
 * @param source_filename The name of the source file to be processed.
 *
 * @return True if there were errors during processing, False otherwise.
 *
 * @remarks The function uses the global variables 'ic' (instruction counter), 'dc' (data counter),
 *          'line_num' (current line number), and 'symbol_table' (global symbol table) to keep track of the processing state.
 */
boolean first_process(char *source_filename) {
    char *modified_filename_macro;
    char line[MAX_LINE_LEN]; /* Buffer to store each line of the source file */
    FILE *extended_src_fd;
    boolean was_error;

    ic = 0;
    dc = 0;
    line_num = 1;
    symbol_table = NULL;
    is_entry_exists = FALSE;
    is_extern_exists = FALSE;

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
            /* Parse and process the line */
            if (!parse_line(line)) {
                was_error = TRUE;
            }
        }

        line_num++;
    }

    /* Update the addresses of symbols in the symbol table */
    update_symbol_addr(symbol_table, MEM_START, INSTRUCTION); /* Update instruction symbols' addresses */
    update_symbol_addr(symbol_table, ic + MEM_START, DIRECTIVE); /* Update directive symbols' addresses */

    free(modified_filename_macro);

    fclose(extended_src_fd);

    return was_error;
}

/**
 * Parse a line of assembly code.
 *
 * @param line The line of assembly code to parse.
 *
 * @return Returns TRUE if the line was parsed successfully, FALSE otherwise.
 *
 * @remarks The function parses a line of assembly code and performs the necessary operations based on the tokens found in the line.
 *          It uses the global variables 'ic' and 'dc' to track the instruction counter and data counter, respectively.
 *          The function also uses the global variable 'symbol_table' to store and manage symbols encountered during parsing.
 */
boolean parse_line(char *line) {
    opcode op_val = NONE_OP;
    directive dir_val = NONE_DIR;
    boolean is_symbol_exists = FALSE;
    Symbolptr current_symbol =  NULL;
    char current_token[MAX_LINE_LEN];

    /* Extract the next token from the line, which could be a symbol or an operation/directive */
    copy_next_token(current_token, line, ":\t ");

    /* If the token is a symbol, add it to the symbol table */
    if (is_symbol(current_token, TRUE)) {
        is_symbol_exists = TRUE;
        current_token[strlen(current_token) - 1] = '\0'; /* Remove the colon from the token */
        current_symbol = add_symbol_to_list(&symbol_table, current_token, DEFAULT_ADDR, FALSE);
        if (current_symbol == NULL) {
            return FALSE;
        }
        line = extract_remaining_seq(line, ":");
        if (is_empty(line)) {
            delete_symbol(&symbol_table, current_symbol->name);
            is_symbol_exists = FALSE;
            print_error(SYMBOL_ONLY);
            return FALSE;
        }
        copy_next_token(current_token, line, ",\t ");
    }

    /* If the token is an operation, process it */
    if ((op_val = find_operation(current_token)) != NONE_OP) {
        if (is_symbol_exists) {
            current_symbol->type = INSTRUCTION;
            current_symbol->address = ic;
        }
        line = extract_remaining_seq(line, ",\t ");
        if (line[0] == ',') {
            if (is_symbol_exists) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            }
            print_error(ILLEGAL_COMMA);
            return FALSE;
        }
        if (has_consecutive_commas(line)) {
            if (is_symbol_exists) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            }
            print_error(CONSECUTIVE_COMMAS);
            return FALSE;
        }
        /* Process the operation */
        if (!process_operation(op_val, line)) {
            if (is_symbol_exists) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            }
            return FALSE;
        }
    /* If the token is a directive, process it */
    } else if ((dir_val = find_directive(current_token)) != NONE_DIR) {
        if (is_symbol_exists) {
            /* Skip symbol creation before encountering .entry/.extern directive */
            if (dir_val == EXTERN || dir_val == ENTRY) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            } else {
                current_symbol->type = DIRECTIVE;
                current_symbol->address = dc;
            }
        }
        line = extract_remaining_seq(line, ",\t ");
        if (line[0] == ',') {
            if (is_symbol_exists) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            }
            print_error(ILLEGAL_COMMA);
            return FALSE;
        }
        if (has_consecutive_commas(line)) {
            if (is_symbol_exists) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            }
            print_error(CONSECUTIVE_COMMAS);
            return FALSE;
        }
        /* Process the directive */
        if (!process_directive(dir_val, line)) {
            if (is_symbol_exists) {
                delete_symbol(&symbol_table, current_symbol->name);
                is_symbol_exists = FALSE;
            }
            return FALSE;
        }
    /* If the token is neither an operation nor a directive, it is undefined */
    } else {
        if (is_symbol_exists) {
            delete_symbol(&symbol_table, current_symbol->name);
            is_symbol_exists = FALSE;
        }
        print_error(UNDEFINED_OP_DIR);
        return FALSE;
    }

    return TRUE;
}

/**
 * Processes an operation in the assembly code.
 *
 * @param op_type   The opcode related to the operation name.
 * @param line      The string representation of a single line of assembly code containing the operation.
 *
 * @return True if the operation is successfully processed, False otherwise.
 *
 * @remarks The function uses the global variable 'ic' (instruction counter) to update the instruction counter based on the additional word count.
 */
boolean process_operation(opcode op_type, char *line) {
    boolean has_first_operand = FALSE, has_second_operand = FALSE;
    addressing_mode first_operand_addr_mode = NONE_ADDR, second_operand_addr_mode = NONE_ADDR;
    char first_operand[MAX_OPERAND_LEN]; /* Represents the source operand or the destination operand (if no second operand is applicable). */
    char second_operand[MAX_OPERAND_LEN]; /* Represents the destination operand if it exists. */
    int commas_cnt = count_commas(line);

    if (commas_cnt > OP_MAX_NUM_COMMAS) {
        /* Too many commas indicate extraneous characters */
        print_error(OP_EXTRANEOUS_COMMA);
        return FALSE;
    } else if (commas_cnt) {
        /* Expected two operands separated by a comma */
        copy_next_token(first_operand, line, ",\t ");
        line = extract_remaining_seq(line, ",\t ");
        if (!is_empty(first_operand)) {
            has_first_operand = TRUE;
            if (line[0] == ',') {
                line++; /* Skip comma */
                copy_next_token(second_operand, line, "\t ");
                line = extract_remaining_seq(line, "\t ");
                if (!is_empty(second_operand)) {
                    has_second_operand = TRUE;
                } else {
                    /* Missing second operand after the comma */
                    print_error(OP_MISSING_OPERAND);
                    return FALSE;
                }
            } else {
                /* Extraneous characters found instead of a comma */
                print_error(OP_EXTRANEOUS_TEXT);
                return FALSE;
            }
        } else {
            /* Missing first operand before the comma */
            print_error(OP_MISSING_OPERAND);
            return FALSE;
        }
    } else {
        /* Single operand or no operands expected */
        copy_next_token(first_operand, line, "\t ");
        line = extract_remaining_seq(line, "\t ");
        if (!is_empty(first_operand)) {
            has_first_operand = TRUE;
        } else if (op_type != RTS_OP && op_type != STOP_OP) {
            /* Missing operand */
            print_error(OP_MISSING_OPERAND);
            return FALSE;
        }
    }

    if (!is_empty(line)) {
        /* Extraneous characters found after the operands */
        print_error(OP_EXTRANEOUS_TEXT);
        return FALSE;
    }

    if (has_first_operand) {
        /* Determine the addressing mode of the first operand */
        first_operand_addr_mode = detect_addr_mode(first_operand);
    }

    if (has_second_operand) {
        /* Determine the addressing mode of the second operand */
        second_operand_addr_mode = detect_addr_mode(second_operand);
    }

    if ((has_first_operand && first_operand_addr_mode == NONE_ADDR) || (has_second_operand && second_operand_addr_mode == NONE_ADDR)) {
        /* Invalid addressing mode detected */
        print_error(OP_INVALID_ADDR_MODE);
        return FALSE;
    }

    if (!is_valid_operand_count(op_type, has_first_operand, has_second_operand)) {
        /* Invalid number of operands for the given operation */
        print_error(OP_INVALID_OPERANDS_NUM);
        return FALSE;
    }

    if (!is_valid_mode_combination(op_type, first_operand_addr_mode, second_operand_addr_mode)) {
        /* Invalid combination of addressing modes for the given operation */
        print_error(OP_INVALID_OPERANDS_MODE);
        return FALSE;
    }

    /* Encode the operation word and append it to the code segment */
    append_word_to_code(encode_first_op_word(op_type, has_first_operand, has_second_operand, first_operand_addr_mode, second_operand_addr_mode));

    /* Update the instruction counter (IC) based on the additional word count */
    ic += get_additional_word_count(has_first_operand, has_second_operand, first_operand_addr_mode, second_operand_addr_mode);

    return TRUE;
}

/**
 * Processes the specified directive based on the given directive type.
 *
 * @param dir_type  The type of the directive to process.
 * @param line      The line of assembly code containing the directive and its parameters.
 *
 * @return TRUE if the directive was processed successfully, FALSE otherwise.
 */
boolean process_directive(directive dir_type, char *line) {
    if (is_empty(line)) {
        print_error(DIR_MISSING_PARAMS);
        return FALSE;
    }

    switch (dir_type) {
        case DATA:
            return process_data_dir(line);
        case STRING:
            return process_string_dir(line);
        case ENTRY:
            return process_entry_dir(line);
        case EXTERN:
            return process_extern_dir(line);
        default:
            break;
    }

    return FALSE;
}

/**
 * Processes the DATA directive by extracting and appending numeric operands to the data segment.
 *
 * @param line The line of assembly code containing the DATA directive and its numeric operands.
 *
 * @return TRUE if the DATA directive was processed successfully, FALSE otherwise.
 */
boolean process_data_dir(char *line) {
    char param[MAX_OPERAND_LEN];

    /* Process each param until the line is empty */
    while (!is_empty(line)) {
        /* Extract the next param */
        copy_next_token(param, line, ",\t ");

        /* Check if the param is a valid number */
        if (!is_number(param)) {
            print_error(DATA_NOT_NUM);
            return FALSE;
        }

        /* Extract the remaining sequence after the param */
        line = extract_remaining_seq(line, ",\t ");

        /* Check for a missing comma between operands */
        if (!is_empty(line) && line[0] != ',') {
            print_error(DATA_MISSING_COMMA);
            return FALSE;
        }

        /* Check for extraneous text after a comma */
        if (is_empty(line + 1) && line[0] == ',') {
            print_error(DATA_EXTRANEOUS_TEXT);
            return FALSE;
        }

        /* Convert the param to a number and append it to the data segment */
        append_number_to_data(atoi(param));

        /* Move to the next character in the line */
        if (!is_empty(line)) {
            line++;
        }
    }

    return TRUE;
}

/**
 * Processes the STRING directive by extracting and appending the characters of a string operand to the data segment.
 *
 * @param line The line of assembly code containing the STRING directive and its string operand.
 *
 * @return TRUE if the STRING directive was processed successfully, FALSE otherwise.
 */
boolean process_string_dir(char *line) {
    char param[MAX_OPERAND_LEN];
    int param_len;
    int i;

    /* Trim any trailing whitespaces from the line */
    trim_end_whitespaces(line);

    /* Extract the string param */
    copy_next_token(param, line, "");

    /* Check if the param is a valid string */
    if (!is_string(param)) {
        print_error(STRING_NOT_STR);
        return FALSE;
    }

    param_len = strlen(param);

    /* Append each character of the string param to the data segment */
    for (i = 1; i < param_len - 1; i++) {
        append_character_to_data(param[i]);
    }

    /* Append a null terminator to mark the end of the string */
    append_character_to_data('\0');

    return TRUE;
}

/**
 * Processes the ENTRY directive by extracting and validating the symbol name specified in the directive.
 *
 * @param line The line of assembly code containing the ENTRY directive and the symbol name.
 *
 * @return TRUE if the ENTRY directive was processed successfully, FALSE otherwise.
 */
boolean process_entry_dir(char *line) {
    char param[MAX_SYMBOL_LEN];

    /* Extract the symbol name from the line */
    copy_next_token(param, line, "\t ");

    /* Check if the symbol name is missing */
    if (is_empty(param)) {
        print_error(ENTRY_MISSING_SYMBOL);
        return FALSE;
    }

    /* Validate the symbol name */
    if (!is_symbol(param, FALSE)) {
        return FALSE;
    }

    /* Extract any remaining sequence from the line */
    line = extract_remaining_seq(line, "\t ");

    /* Check for extraneous text after the symbol name */
    if (!is_empty(line)) {
        print_error(ENTRY_EXTRANEOUS_TEXT);
        return FALSE;
    }

    return TRUE;
}

/**
 * Processes the EXTERN directive by extracting and validating the external symbol name specified in the directive.
 * It adds the external symbol to the symbol table.
 *
 * @param line The line of assembly code containing the EXTERN directive and the symbol name.
 *
 * @return TRUE if the EXTERN directive was processed successfully, FALSE otherwise.
 *
 * @remarks The function uses the global variable 'symbol_table' to store and manage symbols encountered during processing.
 */
boolean process_extern_dir(char *line) {
    char param[MAX_SYMBOL_LEN];

    /* Extract the symbol name from the line */
    copy_next_token(param, line, "\t ");

    /* Check if the symbol name is missing */
    if (is_empty(param)) {
        print_error(EXTERN_MISSING_SYMBOL);
        return FALSE;
    }

    /* Validate the symbol name */
    if (!is_symbol(param, FALSE)) {
        return FALSE;
    }

    /* Extract any remaining sequence from the line */
    line = extract_remaining_seq(line, "\t ");

    /* Check for extraneous text after the symbol name */
    if (!is_empty(line)) {
        print_error(EXTERN_EXTRANEOUS_TEXT);
        return FALSE;
    }

    /* Add the external symbol to the symbol table */
    if (add_symbol_to_list(&symbol_table, param, DEFAULT_ADDR, TRUE) == NULL) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Checks if the given sequence represents a valid number.
 *
 * @param seq The sequence to be checked.
 *
 * @return TRUE if the sequence is a valid number, FALSE otherwise.
 */
boolean is_number(char *seq) {
    if (seq == NULL) {
        return FALSE;
    }

    if (*seq == '+' || *seq == '-') {
        /* Skip the sign character (+/-) */
        seq++;
        if (!isdigit(*seq)) {
            /* If there is no digit after the sign, it's not a valid number */
            return FALSE;
        }
        seq++;
    }

    while (*seq != '\0') {
        if (!isdigit(*seq)) {
            /* If any character is not a digit, it's not a valid number */
            return FALSE;
        }
        seq++;
    }

    return TRUE;
}

/**
 * Checks if the given string is a valid string literal.
 *
 * @param str The string to be checked.
 *
 * @return TRUE if the string is a valid string literal, FALSE otherwise.
 */
boolean is_string(char *str) {
    int len = strlen(str);
    int i;

    /* Check if the string is NULL, has less than 2 characters, or does not start and end with a double quote (") */
    if (str == NULL || len < 2 || str[0] != '\"' || str[len - 1] != '\"') {
        return FALSE;
    }

    /* Iterate through the string, starting from the second character (index 1) and ending at the second last character (index len - 2) */
    for (i = 1; i < len - 1; i++) {
        /* If there is any double quote within the string, it's not a valid string literal */
        if (str[i] == '\"') {
            return FALSE;
        }
    }

    return TRUE;
}

/**
 * Detects the addressing mode of the given operand.
 *
 * @param operand The operand to detect the addressing mode for.
 *
 * @return The addressing mode of the operand.
 */
addressing_mode detect_addr_mode(char *operand) {
    /* If the operand is a number, it has immediate addressing mode */
    if (is_number(operand)) {
        return IMMEDIATE_ADDR;
    /* If the operand is a register, it has register direct addressing mode */
    } else if (is_register(operand)) {
        return REG_DIRECT_ADDR;
    /* If the operand is a valid symbol without a colon, it has direct addressing mode */
    } else if (is_symbol(operand, FALSE)) {
        return DIRECT_ADDR;
    /* If none of the above conditions match, the addressing mode is not recognized */
    } else {
        return NONE_ADDR;
    }
}

/**
 * Checks if the given operation type and operand count are valid.
 *
 * @param op_type       The operation type.
 * @param has_first     Specifies whether the instruction has the first operand.
 * @param has_second    Specifies whether the instruction has the second operand.
 *
 * @return TRUE if the operand count is valid for the given operation type, FALSE otherwise.
 */
boolean is_valid_operand_count(opcode op_type, boolean has_first, boolean has_second) {
    switch (op_type) {
        /* First instructions group: two operands */
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP:
            return has_first && has_second;

            /* Second instructions group: a single operand */
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case RED_OP:
        case PRN_OP:
        case JSR_OP:
            return has_first && !has_second;

            /* Third instructions group: no operands */
        case RTS_OP:
        case STOP_OP:
            return !has_first && !has_second;

        default:
            break;
    }

    return FALSE;
}

/**
 * Checks if the given combination of operation type and addressing modes is valid.
 *
 * @param op_type       The operation type.
 * @param first_mode    The addressing mode of the first operand.
 * @param second_mode   The addressing mode of the second operand.
 *
 * @return TRUE if the combination is valid, FALSE otherwise.
 */
boolean is_valid_mode_combination(opcode op_type, addressing_mode first_mode, addressing_mode second_mode) {
    switch (op_type) {
        /* Source operand addressing modes: all */
        /* Destination operand addressing modes: 3, 5 */
        case MOV_OP:
        case ADD_OP:
        case SUB_OP:
            return (first_mode == IMMEDIATE_ADDR || first_mode == DIRECT_ADDR || first_mode == REG_DIRECT_ADDR) &&
                   (second_mode == DIRECT_ADDR || second_mode == REG_DIRECT_ADDR);

        /* Source operand addressing modes: 3 */
        /* Destination operand addressing modes: 3, 5 */
        case LEA_OP:
            return (first_mode == DIRECT_ADDR) && (second_mode == DIRECT_ADDR || second_mode == REG_DIRECT_ADDR);

        /* Source operand addressing modes: none */
        /* Destination operand addressing modes: 3, 5 */
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case RED_OP:
        case JSR_OP:
            return first_mode == DIRECT_ADDR || first_mode == REG_DIRECT_ADDR;

        /* Source and destination operands addressing modes: all / none (number of operands was verified) */
        case PRN_OP:
        case CMP_OP:
        case RTS_OP:
        case STOP_OP:
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

/**
 * Appends a number to the data segment.
 *
 * @param num The number to be appended.
 *
 * @remarks The function appends the given number to the global array 'data[]', which stores the assembled data.
 *          It maintains the state of the data segment by using the global variable 'dc', which represents the data counter.
 */
void append_number_to_data(int num) {
    data[dc++] = (unsigned int)num;
}

/**
 * Appends a character to the data segment.
 *
 * @param ch The character to be appended.
 *
 * @remarks The function appends the given character to the global array 'data[]', which stores the assembled data.
 *          It maintains the state of the data segment by using the global variable 'dc', which represents the data counter.
 */
void append_character_to_data(char ch) {
    data[dc++] = (unsigned int)ch;
}

/**
 * Gets the additional word count required based on the presence and addressing modes of operands.
 *
 * @param has_first     Specifies whether the instruction has a first operand.
 * @param has_second    Specifies whether the instruction has a second operand.
 * @param first_mode    The addressing mode of the first operand.
 * @param second_mode   The addressing mode of the second operand.
 *
 * @return The additional word count required based on the operands.
 */
int get_additional_word_count(boolean has_first, boolean has_second, addressing_mode first_mode, addressing_mode second_mode) {
    int count = 0;

    /* Increment count for each operand present */
    if (has_first) {
        count++;
    }

    if (has_second) {
        count++;
    }

    /* Decrement count if both operands are REG_DIRECT_ADDR */
    if (has_first && has_second && first_mode == REG_DIRECT_ADDR && second_mode == REG_DIRECT_ADDR) {
        count--;
    }

    return count;
}

/**
 * Encodes the first operand word based on the operation type and operand information.
 *
 * @param op_type       The operation type.
 * @param has_first     Specifies whether the instruction has a first operand.
 * @param has_second    Specifies whether the instruction has a second operand.
 * @param first_mode    The addressing mode of the first operand.
 * @param second_mode   The addressing mode of the second operand.
 *
 * @return The encoded first operand word.
 */
unsigned int encode_first_op_word(opcode op_type, boolean has_first, boolean has_second, addressing_mode first_mode, addressing_mode second_mode) {
    unsigned int word = 0;

    /* First instructions group: Two operands */
    if (has_first && has_second) {
        /* Construct the word with first_mode, op_type, and second_mode */
        word = (first_mode << (OPCODE_BITS + ADDR_MODE_BITS)) | (op_type << ADDR_MODE_BITS) | second_mode;
    /* Second instructions group: Single operand */
    } else if (has_first) {
        /* Construct the word with op_type and first_mode */
        word = (op_type << ADDR_MODE_BITS) | first_mode;
    /* Third instructions group: No operands */
    } else {
        /* Construct the word with op_type only */
        word = op_type << ADDR_MODE_BITS;
    }

    /* Encode the ARE bits by calling the encode_are function */
    word = encode_are(word, ABSOLUTE);

    return word;
}

/**
 * Counts the number of commas in the given string.
 *
 * @param str The string to count commas in.
 *
 * @return The number of commas found in the string.
 */
int count_commas(char *str) {
    int i = 0;
    int count = 0;

    if (str == NULL) {
        return count;
    }

    while (str[i] != '\0') {
        if (str[i] == ',') {
            count++;
        }
        i++;
    }

    return count;
}

/**
 * Checks if the given string contains consecutive commas with or without whitespaces in between.
 *
 * @param str The string to check.
 *
 * @return TRUE if consecutive commas are found, FALSE otherwise.
 */
boolean has_consecutive_commas(char *str) {
    int i = 0;
    boolean comma_found = FALSE;

    while (str[i] != '\0') {
        if (str[i] == ',' && comma_found) {
            /* Found a second comma with whitespaces in between */
            return TRUE;
        } else if (str[i] == ',') {
            comma_found = TRUE;
        } else if (!isspace(str[i])) {
            /* Reset comma_found if a non-whitespace character is found */
            comma_found = FALSE;
        }

        i++;
    }

    return FALSE;
}