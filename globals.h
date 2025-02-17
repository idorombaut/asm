/**
 * This file contains declarations of global variables and data structures used in an assembly language program.
 * These variables and structures are shared across multiple source code files and provide essential information
 * for the assembly process.
 */

#ifndef ASM_GLOBALS_H
#define ASM_GLOBALS_H

#include "symbol_structs.h"

/* A flag that indicates whether there was at least one entry directive in the program */
extern boolean is_entry_exists;

/* A flag that indicates whether there was at least one extern directive in the program */
extern boolean is_extern_exists;

/* Array to store the assembled code instructions */
extern unsigned int code[];

/* Array to store the assembled data values */
extern unsigned int data[];

/* Instruction Counter (IC) represents the current memory address of the instruction being processed */
extern int ic;

/* Data Counter (DC) represents the current memory address of the data being processed */
extern int dc;

/* Line number represents the current line being parsed */
extern int line_num;

/* Symbol Table is a data structure used to store information about symbols encountered in the assembly code */
/* It allows tracking symbols, their addresses, and other relevant data */
extern Symbolptr symbol_table;

/* Ext Table is a data structure used to store information about external symbols encountered in the assembly code */
extern Extptr ext_table;

#endif