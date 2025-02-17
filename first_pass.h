/**
 * This file contains the function prototypes and constants required for
 * processing and analyzing assembly code during the first pass. The first pass
 * typically involves parsing the input code, identifying operations, directives,
 * and operands, and performing initial validations and checks.
 */

#ifndef ASM_FIRST_PASS_H
#define ASM_FIRST_PASS_H

#include "utils.h"

#define DEFAULT_ADDR 0
#define OP_MAX_NUM_COMMAS 1
#define OPCODE_BITS 4
#define ADDR_MODE_BITS 3

boolean first_process(char *);
boolean parse_line(char *);
boolean process_operation(opcode, char *);
boolean process_directive(directive, char *);
boolean process_data_dir(char *);
boolean process_string_dir(char *);
boolean process_entry_dir(char *);
boolean process_extern_dir(char *);
boolean is_number(char *);
boolean is_string(char *);
addressing_mode detect_addr_mode(char *);
boolean is_valid_operand_count(opcode, boolean, boolean);
boolean is_valid_mode_combination(opcode, addressing_mode, addressing_mode);
void append_number_to_data(int);
void append_character_to_data(char);
int get_additional_word_count(boolean, boolean, addressing_mode, addressing_mode);
unsigned int encode_first_op_word(opcode, boolean, boolean, addressing_mode, addressing_mode);
int count_commas(char *);
boolean has_consecutive_commas(char *);

#endif