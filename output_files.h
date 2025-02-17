/**
 * This header file defines function declarations and constants for creating output files in an assembly language program.
 * The output files include the object file, entries file, and externals file. These files store information such as
 * symbol addresses, encoded code and data segments, and entry/external symbols.
 */

#ifndef ASM_OUTPUT_FILES_H
#define ASM_OUTPUT_FILES_H

#include "utils.h"

#define BASE64_ENCODED_STRING_SIZE 2
#define FIRST_HALF_START 0
#define FIRST_HALF_END 5
#define SECOND_HALF_START 6
#define SECOND_HALF_END 11

void create_output_files(char *);
void create_ent_file(FILE *);
void create_ext_file(FILE *);
void create_ob_file(FILE *);
char *convert_to_base64(unsigned int);

#endif