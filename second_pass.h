/**
 * This file contains function prototypes and constants related to the second pass of assembly processing.
 * The second pass performs the actual translation of assembly code into machine code, including symbol resolution,
 * encoding of instructions and operands, and generation of the final machine code.
 */

#ifndef ASM_SECOND_PASS_H
#define ASM_SECOND_PASS_H

#include "utils.h"

#define SRC_MODE_START_POS 9
#define SRC_MODE_END_POS 11
#define DEST_MODE_START_POS 2
#define DEST_MODE_END_POS 4
#define BITS_IN_REG 5
#define SKIP_TO_NUM_REG 2

boolean second_process(char *);
boolean parse_line_second_pass(char *);
boolean process_operation_second_pass(opcode, char *);
void determine_operand(opcode, boolean *, boolean *);
boolean encode_additional_words(char *, char *, boolean, boolean, addressing_mode, addressing_mode);
unsigned int encode_reg(char *, boolean);
boolean encode_operand_to_code(char *, addressing_mode, boolean);
boolean encode_symbol(char *);

#endif