/**
 * This header file contains the declarations of the data structures and functions related to symbol handling
 * in the assembler.
 */

#ifndef ASM_SYMBOL_STRUCTS_H
#define ASM_SYMBOL_STRUCTS_H

#include "utils.h"

/* Forward declaration of the struct symbol */
typedef struct symbol Symbol;

/* Pointer to the struct symbol */
typedef Symbol *Symbolptr;

/* Forward declaration of the struct ext */
typedef struct ext Ext;

/* Pointer to the struct ext */
typedef Ext *Extptr;

/* Enumeration for statement types */
typedef enum statement_type { INSTRUCTION, DIRECTIVE } statement_type;

void update_symbol_addr(Symbolptr, unsigned int, statement_type);
boolean make_entry(Symbolptr, char *);
unsigned int get_symbol_addr(Symbolptr, char *);
boolean is_extern_symbol(Symbolptr, char *);
boolean is_existing_symbol(Symbolptr, char *);
Symbolptr find_symbol(Symbolptr, char *);
Symbolptr create_symbol(char *, unsigned int, boolean);
Symbolptr add_symbol_to_list(Symbolptr *, char *, unsigned int, boolean);
void free_symbol(Symbolptr *);
void delete_symbol(Symbolptr *, char *);
Extptr create_ext(char *, unsigned int);
Extptr add_ext_to_list(Extptr *, char *, unsigned int);
void free_ext(Extptr *);

#endif