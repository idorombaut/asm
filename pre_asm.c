/**
 * This file provides functions for preprocessing an assembly source file.
 * It includes functions for creating and managing macros, expanding macros,
 * and generating an output file with the expanded code.
 */

#include <string.h>
#include <stdlib.h>
#include "pre_asm.h"
#include "utils.h"
#include "globals.h"

/* Definition of the struct mcr */
struct mcr {
    char name[MAX_MCR_LEN]; /* Name of the macro */
    char **lines; /* Array of lines in the macro definition */
    int line_count; /* Number of lines in the macro */
    Mcrptr next; /* Pointer to the next macro in the linked list */
};

/**
 * Creates a new macro with the given name.
 *
 * @param name The name of the macro.
 *
 * @return A pointer to the newly created macro if memory allocation is successful, NULL otherwise.
 */
Mcrptr create_macro(char *name) {
    Mcrptr macro = (Mcrptr)malloc(sizeof(Mcr));
    if (macro == NULL) {
        print_error(MEM_ALLOC_FAILED);
        exit(1);
    }

    strcpy(macro->name, name);
    macro->lines = NULL;
    macro->line_count = 0;
    macro->next = NULL;

    return macro;
}

/**
 * Adds a new macro to the end of the macro linked list.
 *
 * @param head The address of the pointer to the head of the macro linked list.
 * @param name The name of the macro to be added.
 *
 * @return The newly added macro.
 */
Mcrptr add_macro_to_list(Mcrptr *head, char *name) {
    /* Create a new Macro node */
    Mcrptr new_macro = create_macro(name);

    /* If the linked list is empty, make the new node the head */
    if (*head == NULL) {
        *head = new_macro;
    } else {
        /* Traverse to the end of the linked list */
        Mcrptr current = *head;
        while (current->next != NULL) {
            current = current->next;
        }

        /* Set the next pointer of the last node to the new node */
        current->next = new_macro;
    }

    return new_macro;
}

/**
 * Adds a new line to a macro's line array.
 *
 * @param macro The macro to which the line should be added.
 * @param line  The line to be added.
 */
void add_line_to_macro(Mcrptr macro, char *line) {
    char **new_lines;

    /* Increment the line count of the macro */
    macro->line_count++;

    /* Reallocate memory for the expanded line array */
    new_lines = (char **)realloc(macro->lines, macro->line_count * sizeof(char *));
    if (new_lines == NULL) {
        print_error(MEM_REALLOC_FAILED);
        free_macro(&macro);
        exit(1);
    } else {
        /* Update the macro's lines pointer to the new memory block */
        macro->lines = new_lines;

        /* Allocate memory for the new line */
        macro->lines[macro->line_count - 1] = (char *)malloc((strlen(line) + 1) * sizeof(char));
        if (macro->lines[macro->line_count - 1] == NULL) {
            print_error(MEM_ALLOC_FAILED);
            free_macro(&macro);
            exit(1);
        }

        /* Copy the line content into the allocated memory */
        strcpy(macro->lines[macro->line_count - 1], line);
    }
}

/**
 * Frees the memory allocated for a macro, including its lines and the macro itself.
 *
 * @param macro A pointer to the macro pointer.
 */
void free_macro(Mcrptr *macro) {
    int i;

    /* Free memory for each line in the macro */
    for (i = 0; i < (*macro)->line_count; i++) {
        free((*macro)->lines[i]);
    }

    /* Free the memory allocated for the line array */
    free((*macro)->lines);

    /* Free the memory allocated for the macro itself */
    free(*macro);

    /* Set the macro pointer to NULL */
    *macro = NULL;
}

/**
 * Frees the memory allocated for a linked list of macros.
 *
 * @param head A pointer to the head pointer of the linked list.
 */
void free_linked_list(Mcrptr *head) {
    Mcrptr current;
    Mcrptr next;

    if (*head == NULL) {
        return;
    }

    current = *head;

    /* Traverse the linked list and free memory for each macro */
    while (current != NULL) {
        next = current->next;

        /* Free memory for the current macro */
        free_macro(&current);

        /* Move to the next node in the linked list */
        current = next;
    }

    /* Set the head pointer to NULL */
    *head = NULL;
}

/**
 * Finds a macro with the given name in the macro table.
 *
 * @param macro_table   The macro table (linked list) to search in.
 * @param name          The name of the macro to find.
 *
 * @return A pointer to the found macro, or NULL if not found.
 */
Mcrptr find_macro(Mcrptr macro_table, char *name) {
    Mcrptr current_macro = macro_table;

    /* Traverse the macro table (linked list) */
    while (current_macro != NULL) {
        /* Compare the name of the current macro with the target name */
        if (strcmp(current_macro->name, name) == 0) {
            /* Found a match, return the pointer to the current macro */
            return current_macro;
        }

        /* Move to the next macro in the macro table */
        current_macro = current_macro->next;
    }

    /* Macro not found, return NULL */
    return NULL;
}

/**
 * Expands a macro by writing its lines to the output file.
 *
 * @param output_file   The pointer to the output file where the expanded macro lines will be written.
 * @param macro_table   The pointer to the macro table (linked list) containing all the defined macros.
 * @param macro_name    The name of the macro to be expanded.
 */
void expand_macro(FILE *output_file, Mcrptr macro_table, char *macro_name) {
    /* Find the macro with the given name in the macro table */
    Mcrptr macro = find_macro(macro_table, macro_name);
    int i;

    /* Write each line of the macro to the output file */
    for (i = 0; i < macro->line_count; i++) {
        fprintf(output_file, "%s", macro->lines[i]);
    }
}

/**
 * Checks if a given token is a valid macro name.
 *
 * @param token The token to be checked as a macro name.
 *
 * @return TRUE if the token is a valid macro name, otherwise returns FALSE.
 */
boolean is_macro(char *token) {
    int token_len = strlen(token);

    if (token == NULL) {
        return FALSE;
    }

    /* Check if the token length exceeds the maximum allowed macro length */
    if (token_len > MAX_MCR_LEN) {
        print_error(MCR_TOO_LONG);
        return FALSE;
    }

    /* Check if the token is a register name */
    if (is_register(token)) {
        print_error(MCR_CANNOT_BE_REG);
        return FALSE;
    }

    /* Check if the token matches any operation name */
    if (find_operation(token) != NONE_OP) {
        print_error(MCR_CANNOT_BE_OP);
        return FALSE;
    }

    /* Check if the token matches any directive name */
    if (find_directive(token) != NONE_DIR) {
        print_error(MCR_CANNOT_BE_DIR);
        return FALSE;
    }

    return TRUE;
}

/**
 * Performs preprocessing on a source file, expanding macros and generating an output file.
 *
 * @param source_filename The filename of the source file to be processed.
 *
 * @return TRUE if the preprocessing is successful, FALSE otherwise.
 */
boolean pre_process(char *source_filename) {
    char *modified_filename_source;
    char *modified_filename_macro;
    char *line;
    char *trimmed_line;
    char *macro_name;
    FILE *initial_src_fd;
    FILE *extended_src_fd;
    Mcrptr macro_table;
    Mcrptr current_macro;
    boolean is_inside_macro;
    boolean success;

    line_num = 1;

    macro_table = NULL;
    current_macro = NULL; /* Pointer to the currently processed macro */
    is_inside_macro = FALSE; /* Flag indicating if we're inside a macro definition */
    success = TRUE; /* Flag to track the success of the process */

    modified_filename_source = generate_new_filename(source_filename, FILE_SOURCE);
    modified_filename_macro = generate_new_filename(source_filename, FILE_MACRO);

    line = (char *)malloc(MAX_LINE_LEN * sizeof(char));
    if (line == NULL) {
        print_error(MEM_ALLOC_FAILED);
        free(modified_filename_source);
        free(modified_filename_macro);
        return FALSE;
    }

    trimmed_line = (char *)malloc(MAX_LINE_LEN * sizeof(char));
    if (trimmed_line == NULL) {
        print_error(MEM_ALLOC_FAILED);
        free(modified_filename_source);
        free(modified_filename_macro);
        free(line);
        return FALSE;
    }

    macro_name = (char *)malloc(MAX_MCR_LEN * sizeof(char));
    if (macro_name == NULL) {
        print_error(MEM_ALLOC_FAILED);
        free(modified_filename_source);
        free(modified_filename_macro);
        free(line);
        free(trimmed_line);
        return FALSE;
    }

    initial_src_fd = fopen(modified_filename_source, "r");
    if (initial_src_fd == NULL) {
        print_error(CANNOT_OPEN_FILE);
        free(modified_filename_source);
        free(modified_filename_macro);
        free(line);
        free(trimmed_line);
        free(macro_name);
        return FALSE;
    }

    extended_src_fd = fopen(modified_filename_macro, "w");
    if (extended_src_fd == NULL) {
        print_error(CANNOT_CREATE_FILE);
        free(modified_filename_source);
        free(modified_filename_macro);
        free(line);
        free(trimmed_line);
        free(macro_name);
        fclose(initial_src_fd);
        return FALSE;
    }

    /* Process each line of the source file */
    while (fgets(line, MAX_LINE_LEN, initial_src_fd)) {
        /* Create a trimmed copy of the line */
        strcpy(trimmed_line, line);
        trim_whitespaces(trimmed_line);

        /* Check if the line is a macro definition */
        if (strncmp(trimmed_line, "mcro", 4) == 0) {
            /* Extract the macro name */
            char *token = strtok(trimmed_line + 4, " ");

            if (token == NULL) {
                print_error(MCR_MISSING_NAME);
                success = FALSE;
                break;
            }

            strcpy(macro_name, token);

            token = strtok(NULL, " ");
            if (token != NULL) {
                print_error(MCR_MCRO_EXTRANEOUS_TEXT);
                success = FALSE;
                break;
            }

            /* Add the macro to the macro table */
            if (is_macro(macro_name)) {
                current_macro = add_macro_to_list(&macro_table, macro_name);
                is_inside_macro = TRUE;
            } else {
                success = FALSE;
                break;
            }
        /* Check if we're inside a macro and encountering an "endmcro" line */
        } else if (is_inside_macro && strncmp(trimmed_line, "endmcro", 7) == 0) {
            char *token = strtok(trimmed_line + 7, " ");

            if (token != NULL) {
                print_error(MCR_ENDMCRO_EXTRANEOUS_TEXT);
                success = FALSE;
                break;
            }

            current_macro = NULL;
            is_inside_macro = FALSE;
        /* Check if we're inside a macro and need to add the line to the macro's definition */
        } else if (is_inside_macro) {
            add_line_to_macro(current_macro, line);
        /* Check if the line matches any defined macro */
        } else if (find_macro(macro_table, trimmed_line) != NULL) {
            /* Expand the macro and write it to the output file */
            expand_macro(extended_src_fd, macro_table, trimmed_line);
        /* The line is not a macro, write it to the output file as is */
        } else {
            fprintf(extended_src_fd, "%s", line);
        }

        line_num++;
    }

    free_linked_list(&macro_table);

    free(line);
    free(trimmed_line);
    free(macro_name);

    fclose(initial_src_fd);
    fclose(extended_src_fd);

    /* Clean up in case of failure */
    if (!success) {
        boolean remove_result = remove(modified_filename_macro);
        if (remove_result != FALSE) {
            print_error(CANNOT_DELETE_FILE);
        }
    }

    free(modified_filename_source);
    free(modified_filename_macro);

    return success; /* Return the flag indicating success or failure */
}