/**
 * This file contains utility functions and various definitions used in the assembly
 * language project. These utilities are designed to facilitate parsing, error handling, and other
 * essential tasks within the assembly language assembler.
 */

#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include <stdio.h>

#define MAX_LINE_LEN 81
#define MAX_SYMBOL_LEN 31
#define MAX_OPERAND_LEN 31
#define MAX_EXTENSION_LEN 4
#define MEM_SIZE 1024
#define MEM_START 100
#define REG_LEN 3
#define MIN_REG_INDEX 0
#define MAX_REG_INDEX 7
#define ARE_BITS 2

/* Enumeration for file types */
typedef enum file_type {
    FILE_SOURCE,
    FILE_MACRO,
    FILE_OBJECT,
    FILE_ENTRIES,
    FILE_EXTERNALS
} file_type;

/* Enumeration for error types */
typedef enum err {
    NOT_ENOUGH_PARAMS,
    MCR_EXP_FAILED,
    FIRST_PASS_FAILED,
    SECOND_PASS_FAILED,
    MEM_ALLOC_FAILED,
    MEM_REALLOC_FAILED,
    CANNOT_OPEN_FILE,
    CANNOT_CREATE_FILE,
    CANNOT_DELETE_FILE,
    MCR_TOO_LONG,
    MCR_CANNOT_BE_REG,
    MCR_CANNOT_BE_OP,
    MCR_CANNOT_BE_DIR,
    MCR_MISSING_NAME,
    MCR_MCRO_EXTRANEOUS_TEXT,
    MCR_ENDMCRO_EXTRANEOUS_TEXT,
    SYMBOL_ONLY,
    ILLEGAL_COMMA,
    CONSECUTIVE_COMMAS,
    UNDEFINED_OP_DIR,
    OP_EXTRANEOUS_COMMA,
    OP_MISSING_OPERAND,
    OP_EXTRANEOUS_TEXT,
    OP_INVALID_ADDR_MODE,
    OP_INVALID_OPERANDS_NUM,
    OP_INVALID_OPERANDS_MODE,
    DIR_MISSING_PARAMS,
    DATA_NOT_NUM,
    DATA_MISSING_COMMA,
    DATA_EXTRANEOUS_TEXT,
    STRING_NOT_STR,
    ENTRY_MISSING_SYMBOL,
    ENTRY_EXTRANEOUS_TEXT,
    EXTERN_MISSING_SYMBOL,
    EXTERN_EXTRANEOUS_TEXT,
    SYMBOL_TOO_LONG,
    SYMBOL_CANNOT_BE_REG,
    SYMBOL_CANNOT_BE_OP,
    SYMBOL_CANNOT_BE_DIR,
    SYMBOL_INVALID_FIRST_CHAR,
    SYMBOL_INVALID_CHAR,
    ENTRY_CANNOT_BE_EXTERN,
    ENTRY_SYMBOL_NOT_FOUND,
    SYMBOL_ALREADY_EXISTS,
    SYMBOL_NOT_FOUND
} err;

/* Enumeration for boolean values */
typedef enum boolean { FALSE, TRUE } boolean;

/* Enumeration for ARE (Absolute, External, Relocatable) values */
typedef enum are { ABSOLUTE, EXTERNAL, RELOCATABLE } are;

/* Enumeration for addressing modes */
typedef enum addressing_mode { IMMEDIATE_ADDR = 1, DIRECT_ADDR = 3, REG_DIRECT_ADDR = 5, NONE_ADDR = -1 } addressing_mode;

/* Enumeration for opcode values */
typedef enum opcode {
    MOV_OP,
    CMP_OP,
    ADD_OP,
    SUB_OP,
    NOT_OP,
    CLR_OP,
    LEA_OP,
    INC_OP,
    DEC_OP,
    JMP_OP,
    BNE_OP,
    RED_OP,
    PRN_OP,
    JSR_OP,
    RTS_OP,
    STOP_OP,
    NONE_OP = -1
} opcode;

/* Enumeration for directive values */
typedef enum directive { DATA, STRING, ENTRY, EXTERN, NONE_DIR = -1 } directive;

char *generate_new_filename(char *, file_type);
void print_error(err);
void trim_whitespaces(char *);
char *skip_whitespaces(char *);
void trim_end_whitespaces(char *);
boolean is_empty(char *);
boolean should_ignore(char *);
boolean is_separator(char, char *);
void copy_next_token(char *, char *, char *);
char *extract_remaining_seq(char *, char *);
boolean is_register(char *);
boolean is_symbol(char *, boolean);
opcode find_operation(char *);
directive find_directive(char *);
void append_word_to_code(unsigned int);
unsigned int encode_are(unsigned int, are);
unsigned int extract_bits(unsigned int, int, int);

#endif