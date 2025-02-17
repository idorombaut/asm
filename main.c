/**
 * This file contains the main function and related code that serves as the entry point for the software project.
 * It performs various tasks, including command-line argument processing, two passes of processing on each argument,
 * creating output files, and freeing allocated memory.
 */

#include "utils.h"
#include "pre_asm.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output_files.h"
#include "symbol_structs.h"

boolean is_entry_exists; /* Flag to track if an entry exists in the code */
boolean is_extern_exists; /* Flag to track if an extern declaration exists in the code */
unsigned int code[MEM_SIZE]; /* Array to store the machine code instructions */
unsigned int data[MEM_SIZE]; /* Array to store the data values */
int ic; /* Instruction Counter */
int dc; /* Data Counter */
int line_num; /* Line number in the input file */
Symbolptr symbol_table; /* Pointer to the symbol table */
Extptr ext_table; /* Pointer to the extern table */

/**
 * The main entry point of the program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings representing the command-line arguments.
 *
 * @return An integer indicating the exit status of the program.
 */
int main(int argc, char *argv[]) {
    int i;

    /* Check if the number of command-line arguments is less than 2 */
    if (argc < 2) {
        print_error(NOT_ENOUGH_PARAMS);
        return 1;
    }

    /* Iterate over the command-line arguments */
    for (i = 1; i < argc; i++) {
        boolean first_success = TRUE;
        boolean second_success = TRUE;

        /* Pre-process the current argument */
        if (!(pre_process(argv[i]))) {
            print_error(MCR_EXP_FAILED);
            continue;
        }

        /* Perform the first processing pass on the current argument */
        if (first_process(argv[i])) {
            print_error(FIRST_PASS_FAILED);
            first_success = FALSE;
        }

        /* Perform the second processing pass on the current argument */
        if (second_process(argv[i])) {
            print_error(SECOND_PASS_FAILED);
            second_success = FALSE;
            continue;
        }

        /* Only if all passes succeeded write the .ob, .ent, and .ext output files for the current argument */
        if (first_success && second_success) {
            create_output_files(argv[i]);
        }

        /* Free the memory used by the symbol table */
        free_symbol(&symbol_table);

        /* Free the memory used by the extern table */
        free_ext(&ext_table);
    }

    return 0;
}