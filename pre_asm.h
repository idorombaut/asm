/**
 * This header file declares functions and types used for preprocessing an
 * assembly source file. It includes functions for creating and managing macros,
 * expanding macros, and performing the preprocessing operation.
 */

#ifndef ASM_PRE_ASM_H
#define ASM_PRE_ASM_H

#include "utils.h"

#define MAX_MCR_LEN 31

/* Forward declaration of the struct mcr */
typedef struct mcr Mcr;

/* Pointer to the struct mcr */
typedef Mcr *Mcrptr;

Mcrptr create_macro(char *);
Mcrptr add_macro_to_list(Mcrptr *, char *);
void add_line_to_macro(Mcrptr, char *);
void free_macro(Mcrptr *);
void free_linked_list(Mcrptr *);
Mcrptr find_macro(Mcrptr, char *);
void expand_macro(FILE *, Mcrptr, char *);
boolean pre_process(char *);

#endif