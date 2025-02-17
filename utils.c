/**
 * This file contains various utility functions that are used in the assembly language
 * processing program. The utility functions help with string manipulation, error handling, and
 * checking the validity of symbols, registers, and other elements in the assembly code.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "globals.h"

/**
 * Generates a new modified file name based on the original file name and the specified file type.
 *
 * @param original  The original file name.
 * @param type      The file type to append to the original file name.
 *
 * @return A dynamically allocated string representing the modified file name.
 */
char *generate_new_filename(char *original, file_type type) {
    char *modified_file_name = (char *)malloc((strlen(original) + MAX_EXTENSION_LEN + 1) * sizeof(char));
    if (modified_file_name == NULL) {
        print_error(MEM_ALLOC_FAILED);
        exit(1);
    }

    strcpy(modified_file_name, original);

    switch (type) {
        case FILE_SOURCE:
            strcat(modified_file_name, ".as");
            break;
        case FILE_MACRO:
            strcat(modified_file_name, ".am");
            break;
        case FILE_OBJECT:
            strcat(modified_file_name, ".ob");
            break;
        case FILE_ENTRIES:
            strcat(modified_file_name, ".ent");
            break;
        case FILE_EXTERNALS:
            strcat(modified_file_name, ".ext");
            break;
        default:
            break;
    }

    return modified_file_name;
}

/**
 * Prints an error message based on the given error code.
 *
 * @param error The error code to be handled and printed.
 *
 * @remarks The global variable 'line_num' should be set to the line number where the error occurred.
 */
void print_error(err error) {
    switch (error) {
        case NOT_ENOUGH_PARAMS:
            printf("ERROR: Not enough parameters\n");
            break;
        case MCR_EXP_FAILED:
            printf("ERROR: Macro expansion failed\n");
            break;
        case FIRST_PASS_FAILED:
            printf("ERROR: First pass failed\n");
            break;
        case SECOND_PASS_FAILED:
            printf("ERROR: Second pass failed\n");
            break;
        case MEM_ALLOC_FAILED:
            printf("ERROR: Memory allocation failed\n");
            break;
        case MEM_REALLOC_FAILED:
            printf("ERROR: Memory reallocation failed\n");
            break;
        case CANNOT_OPEN_FILE:
            printf("ERROR: Cannot open file\n");
            break;
        case CANNOT_CREATE_FILE:
            printf("ERROR: Cannot create file\n");
            break;
        case CANNOT_DELETE_FILE:
            printf("ERROR: Cannot delete file\n");
            break;
        case MCR_TOO_LONG:
            printf("ERROR at line %d: Macro name is too long\n", line_num);
            break;
        case MCR_CANNOT_BE_REG:
            printf("ERROR at line %d: Macro name cannot be a register name\n", line_num);
            break;
        case MCR_CANNOT_BE_OP:
            printf("ERROR at line %d: Macro name cannot be an operation name\n", line_num);
            break;
        case MCR_CANNOT_BE_DIR:
            printf("ERROR at line %d: Macro name cannot be directive name\n", line_num);
            break;
        case MCR_MISSING_NAME:
            printf("ERROR at line %d: Missing macro name\n", line_num);
            break;
        case MCR_MCRO_EXTRANEOUS_TEXT:
            printf("ERROR at line %d: Extraneous text after mcro\n", line_num);
            break;
        case MCR_ENDMCRO_EXTRANEOUS_TEXT:
            printf("ERROR at line %d: Extraneous text after endmcro\n", line_num);
            break;
        case SYMBOL_ONLY:
            printf("ERROR at line %d: Only a symbol name is provided\n", line_num);
            break;
        case ILLEGAL_COMMA:
            printf("ERROR at line %d: Illegal comma\n", line_num);
            break;
        case CONSECUTIVE_COMMAS:
            printf("ERROR at line %d: Consecutive commas\n", line_num);
            break;
        case UNDEFINED_OP_DIR:
            printf("ERROR at line %d: Undefined operation or directive encountered\n", line_num);
            break;
        case OP_EXTRANEOUS_COMMA:
            printf("ERROR at line %d: Extraneous comma\n", line_num);
            break;
        case OP_MISSING_OPERAND:
            printf("ERROR at line %d: Missing operand\n", line_num);
            break;
        case OP_EXTRANEOUS_TEXT:
            printf("ERROR at line %d: Extraneous text after operation\n", line_num);
            break;
        case OP_INVALID_ADDR_MODE:
            printf("ERROR at line %d: Invalid addressing mode\n", line_num);
            break;
        case OP_INVALID_OPERANDS_NUM:
            printf("ERROR at line %d: Invalid number of operands\n", line_num);
            break;
        case OP_INVALID_OPERANDS_MODE:
            printf("ERROR at line %d: Invalid operands' addressing mode combination\n", line_num);
            break;
        case DIR_MISSING_PARAMS:
            printf("ERROR at line %d: Directive missing parameters\n", line_num);
            break;
        case DATA_NOT_NUM:
            printf("ERROR at line %d: .data argument is not a valid number\n", line_num);
            break;
        case DATA_MISSING_COMMA:
            printf("ERROR at line %d: .data missing comma\n", line_num);
            break;
        case DATA_EXTRANEOUS_TEXT:
            printf("ERROR at line %d: Extraneous text after .data argument\n", line_num);
            break;
        case STRING_NOT_STR:
            printf("ERROR at line %d: .string argument is not a valid string\n", line_num);
            break;
        case ENTRY_MISSING_SYMBOL:
            printf("ERROR at line %d: .entry missing symbol\n", line_num);
            break;
        case ENTRY_EXTRANEOUS_TEXT:
            printf("ERROR at line %d: Extraneous text after .entry argument\n", line_num);
            break;
        case EXTERN_MISSING_SYMBOL:
            printf("ERROR at line %d: .extern missing symbol\n", line_num);
            break;
        case EXTERN_EXTRANEOUS_TEXT:
            printf("ERROR at line %d: Extraneous text after .extern argument\n", line_num);
            break;
        case SYMBOL_TOO_LONG:
            printf("ERROR at line %d: Symbol name is too long\n", line_num);
            break;
        case SYMBOL_CANNOT_BE_REG:
            printf("ERROR at line %d: Symbol name cannot be a register name\n", line_num);
            break;
        case SYMBOL_CANNOT_BE_OP:
            printf("ERROR at line %d: Symbol name cannot be an operation name\n", line_num);
            break;
        case SYMBOL_CANNOT_BE_DIR:
            printf("ERROR at line %d: Symbol name cannot be a directive name\n", line_num);
            break;
        case SYMBOL_INVALID_FIRST_CHAR:
            printf("ERROR at line %d: Symbol name must start with an alphabetic character\n", line_num);
            break;
        case SYMBOL_INVALID_CHAR:
            printf("ERROR at line %d: Symbol name contains an invalid character. Only alphabetic characters and digits are allowed\n", line_num);
            break;
        case ENTRY_CANNOT_BE_EXTERN:
            printf("ERROR at line %d: Symbol marked as .entry cannot also be .extern\n", line_num);
            break;
        case ENTRY_SYMBOL_NOT_FOUND:
            printf("ERROR at line %d: Entry symbol not found in the symbol table\n", line_num);
            break;
        case SYMBOL_ALREADY_EXISTS:
            printf("ERROR at line %d: Symbol already exists in the symbol table\n", line_num);
            break;
        case SYMBOL_NOT_FOUND:
            printf("ERROR at line %d: Symbol not found in the symbol table\n", line_num);
            break;
        default:
            break;
    }
}

/**
 * Trims leading and trailing whitespaces from a string.
 *
 * @param str The string to be trimmed (null-terminated).
 */
void trim_whitespaces(char *str) {
    int len = strlen(str);
    int left = 0 , right, new_len, i;

    if (str == NULL) {
        return;
    }

    if (str[0] == '\0') {
        return;
    }

    /* Find the index of the first non-whitespace character from the left */
    while (isspace(str[left])) {
        left++;
    }

    /* Find the index of the first non-whitespace character from the right */
    right = len - 1;
    while (right >= left && isspace(str[right])) {
        right--;
    }

    new_len = right - left + 1;

    /* If there are leading whitespaces, shift the characters to the left */
    if (left > 0) {
        for (i = 0; i < new_len; i++) {
            str[i] = str[left + i];
        }
    }

    str[new_len] = '\0';
}

/**
 * Skips leading whitespace characters in the given string and returns a pointer to the first non-whitespace character.
 *
 * @param str The string to skip whitespace characters in.
 *
 * @return A pointer to the first non-whitespace character in the string.
 */
char *skip_whitespaces(char *str) {
    if (str == NULL) {
        return NULL;
    }

    while (isspace(*str) && *str != '\0') {
        /* Advance the pointer while the current character is whitespace */
        str++;
    }

    return str;
}

/**
 * Trims trailing whitespace characters from the end of the given string by modifying it in-place.
 *
 * @param str The string to trim trailing whitespace characters from.
 */
void trim_end_whitespaces(char *str) {
    int str_len = strlen(str);
    int i;

    for (i = str_len - 1; i >= 0; i--) {
        if (!isspace(str[i])) {
            /* Found a non-whitespace character, stop trimming */
            break;
        }
        /* Replace whitespace with null terminator */
        str[i] = '\0';
    }
}

/**
 * Checks if the given string is empty or consists of only whitespace characters.
 *
 * @param line The string to check.
 *
 * @return TRUE if the string is empty or contains only whitespace characters, FALSE otherwise.
 */
boolean is_empty(char *line) {
    /* Skip leading whitespace characters */
    line = skip_whitespaces(line);

    if (line == NULL || *line == '\0') {
        /* If the resulting string is NULL or empty, it is considered empty */
        return TRUE;
    }

    return FALSE;
}

/**
 * Checks if the given line should be ignored during assembly processing.
 *
 * @param line The line of code to check.
 *
 * @return TRUE if the line should be ignored, FALSE otherwise.
 */
boolean should_ignore(char *line) {
    /* If the line is NULL, it should be ignored */
    if (line == NULL) {
        return TRUE;
    }

    /* Skip leading whitespace characters */
    line = skip_whitespaces(line);

    /* If the line starts with a semicolon (;), it is a comment and should be ignored */
    /* If the line is empty, it should be ignored */
    if (is_empty(line) || *line == ';') {
        return TRUE;
    }

    return FALSE;
}

/**
 * Checks if the given character is a separator character by comparing it with the characters in the separator string.
 *
 * @param c     The character to check.
 * @param seps  The separator string containing the separator characters.
 *
 * @return TRUE if the character is a separator, FALSE otherwise.
 */
boolean is_separator(char c, char *seps) {
    int i;

    for (i = 0; seps[i] != '\0'; i++) {
        if (c == seps[i]) {
            /* Found a separator character */
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Copies the next token from the source string to the destination string, delimited by the characters in the separator string.
 * Leading and trailing whitespaces are skipped in the source string before extracting the token.
 *
 * @param dest  The destination string to copy the token to.
 * @param src   The source string to extract the token from.
 * @param seps  The separator string containing the delimiter characters.
 */
void copy_next_token(char *dest, char *src, char *seps) {
    int i = 0;

    if (dest == NULL || src == NULL || seps == NULL) {
        /* Invalid input, return */
        return;
    }

    src = skip_whitespaces(src);

    if (is_empty(src)) {
        /* Empty or whitespace-only source, set destination as empty */
        dest[0] = '\0';
        return;
    }

    while (!is_separator(src[i], seps) && src[i] != '\0') {
        /* Copy characters from source to destination until a separator or null terminator is encountered */
        dest[i] = src[i];
        i++;
    }

    if (src[i] == ':') {
        /* If the last character in the token is a colon, include it in the destination */
        dest[i] = src[i];
        dest[i + 1] = '\0';
    } else {
        /* Set null terminator at the end of the destination to terminate the copied token */
        dest[i] = '\0';
    }
}

/**
 * Extracts the remaining sequence from the input string starting from the current position, excluding any leading whitespaces.
 * The extraction stops when a separator character or null terminator is encountered.
 *
 * @param seq   The input string containing the sequence.
 * @param seps  The separator string containing the delimiter characters.
 *
 * @return  A pointer to the remaining sequence in the input string, excluding leading whitespaces and the separator character.
 *          If the input string is empty, reaches the end, or contains only whitespaces, returns NULL.
 */
char *extract_remaining_seq(char *seq, char *seps) {
    if (seq == NULL || seps == NULL) {
        /* Invalid input, return NULL */
        return NULL;
    }

    seq = skip_whitespaces(seq);

    if (is_empty(seq)) {
        /* Empty or whitespace-only input, return NULL */
        return NULL;
    }

    while (!is_separator(*seq, seps) && *seq != '\0') {
        /* Advance the sequence pointer until a separator or null terminator is encountered */
        seq++;
    }

    if (*seq == ':') {
        /* If the separator is a colon, advance the sequence pointer past it */
        seq++;
    }

    seq = skip_whitespaces(seq);

    return seq;
}

/**
 * Checks if the given token represents a valid register.
 *
 * @param token The token to be checked.
 *
 * @return TRUE if the token is a valid register, FALSE otherwise.
 */
boolean is_register(char *token) {
    if (strlen(token) == REG_LEN && token[0] == '@' && token[1] == 'r' && isdigit(token[2])) {
        int reg_index = token[2] - '0';
        if (reg_index >= MIN_REG_INDEX && reg_index <= MAX_REG_INDEX) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Checks if the given token is a valid symbol.
 *
 * @param token             The token to be checked.
 * @param is_colon_expected Specifies whether a colon (:) is expected at the end of the token.
 *
 * @return TRUE if the token is a valid symbol, FALSE otherwise.
 */
boolean is_symbol(char *token, boolean is_colon_expected) {
    int i;
    int token_len = strlen(token);

    char *token_copy = (char *)malloc((token_len + 1) * sizeof(char));
    if (token_copy == NULL) {
        print_error(MEM_ALLOC_FAILED);
        return FALSE;
    }

    if (token == NULL) {
        return FALSE;
    }

    strcpy(token_copy, token);

    /* Check if a colon (:) exists at the end of the token, if required */
    if (is_colon_expected && token_copy[token_len - 1] != ':') {
        return FALSE;
    }

    /* Remove the colon from the token if it exists */
    if (is_colon_expected) {
        token_copy[token_len - 1] = '\0';
        token_len--;
    }

    /* Check if the token length exceeds the maximum symbol length */
    if (token_len > MAX_SYMBOL_LEN) {
        print_error(SYMBOL_TOO_LONG);
        return FALSE;
    }

    /* Check if the token is a register name */
    if (is_register(token_copy)) {
        print_error(SYMBOL_CANNOT_BE_REG);
        return FALSE;
    }

    /* Check if the token matches any operation name */
    if (find_operation(token_copy) != NONE_OP) {
        print_error(SYMBOL_CANNOT_BE_OP);
        return FALSE;
    }

    /* Check if the token matches any directive name */
    if (find_directive(token_copy) != NONE_DIR) {
        print_error(SYMBOL_CANNOT_BE_DIR);
        return FALSE;
    }

    /* Check if the first character of the token is alphabetic */
    if (!isalpha(token_copy[0])) {
        print_error(SYMBOL_INVALID_FIRST_CHAR);
        return FALSE;
    }

    /* Check if any character in the token is not alphanumeric */
    for (i = 1; i < token_len; i++) {
        if (!isalnum(token_copy[i])) {
            print_error(SYMBOL_INVALID_CHAR);
            return FALSE;
        }
    }

    return TRUE;
}

/**
 * Finds the opcode corresponding to the given operation name.
 *
 * @param op_name The name of the operation to find the opcode for.
 *
 * @return The corresponding opcode if found, or NONE_OP if no matching opcode is found.
 */
opcode find_operation(char *op_name) {
    if (strcmp(op_name, "mov") == 0) {
        return MOV_OP;
    } else if (strcmp(op_name, "cmp") == 0) {
        return CMP_OP;
    } else if (strcmp(op_name, "add") == 0) {
        return ADD_OP;
    } else if (strcmp(op_name, "sub") == 0) {
        return SUB_OP;
    } else if (strcmp(op_name, "not") == 0) {
        return NOT_OP;
    } else if (strcmp(op_name, "clr") == 0) {
        return CLR_OP;
    } else if (strcmp(op_name, "lea") == 0) {
        return LEA_OP;
    } else if (strcmp(op_name, "inc") == 0) {
        return INC_OP;
    } else if (strcmp(op_name, "dec") == 0) {
        return DEC_OP;
    } else if (strcmp(op_name, "jmp") == 0) {
        return JMP_OP;
    } else if (strcmp(op_name, "bne") == 0) {
        return BNE_OP;
    } else if (strcmp(op_name, "red") == 0) {
        return RED_OP;
    } else if (strcmp(op_name, "prn") == 0) {
        return PRN_OP;
    } else if (strcmp(op_name, "jsr") == 0) {
        return JSR_OP;
    } else if (strcmp(op_name, "rts") == 0) {
        return RTS_OP;
    } else if (strcmp(op_name, "stop") == 0) {
        return STOP_OP;
    } else {
        return NONE_OP;
    }
}

/**
 * Finds the directive type corresponding to the given directive name.
 *
 * @param dir_name The name of the directive to find the type for.
 *
 * @return The corresponding directive type if found, or NONE_DIR if no matching directive type is found.
 */
directive find_directive(char *dir_name) {
    if (strcmp(dir_name, ".data") == 0) {
        return DATA;
    } else if (strcmp(dir_name, ".string") == 0) {
        return STRING;
    } else if (strcmp(dir_name, ".entry") == 0) {
        return ENTRY;
    } else if (strcmp(dir_name, ".extern") == 0) {
        return EXTERN;
    } else {
        return NONE_DIR;
    }
}

/**
 * Appends a word to the code segment.
 *
 * @param word The word to be appended.
 *
 * @remarks The function appends the given word to the global array 'code[]', which stores the assembled code instructions.
 *          It also updates the global variable 'ic' (instruction counter) to reflect the appended word.
 */
void append_word_to_code(unsigned int word) {
    code[ic++] = word;
}

/**
 * Encodes the ARE (Absolute/Relocatable/External) bits into the given word.
 *
 * @param word      The word to encode the ARE bits into.
 * @param are_val   The value representing the ARE bits.
 *
 * @return The word with the encoded ARE bits.
 */
unsigned int encode_are(unsigned int word, are are_val) {
    return (word << ARE_BITS) | are_val;
}

/**
 * Extracts a range of bits from an unsigned integer.
 *
 * @param word  The unsigned integer from which to extract bits.
 * @param start The starting position of the range.
 * @param end   The ending position of the range.
 *
 * @return The extracted bits as an unsigned integer.
 */
unsigned int extract_bits(unsigned int word, int start, int end) {
    /* Calculate the number of bits to extract */
    int num_bits = end - start + 1;

    /* Create a mask to extract the desired bits */
    unsigned int mask = (1 << num_bits) - 1;

    /* Shift the word to the right by the starting position and apply the mask */
    unsigned int result = (word >> start) & mask;

    return result;
}