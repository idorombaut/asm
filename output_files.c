/**
 * This file contains the implementation of functions related to creating output files in an assembly language program.
 * The output files include the object file, entries file, and externals file. These files store information such as
 * symbol addresses, encoded code and data segments, and entry/external symbols.
 */

#include <string.h>
#include <stdlib.h>
#include "output_files.h"
#include "utils.h"
#include "globals.h"
#include "symbol_structs.h"

/* Definition of a symbol in the symbol table (linked list) */
struct symbol {
    char name[MAX_SYMBOL_LEN]; /* The name of the symbol */
    unsigned int address; /* The address associated with the symbol */
    statement_type type; /* The type of statement the symbol belongs to */
    boolean is_ext; /* Indicates if the symbol is an external symbol */
    boolean is_ent; /* Indicates if the symbol is an entry symbol */
    Symbolptr next; /* Pointer to the next symbol in the symbol table */
};

/* Definition of an external symbol in the ext table (linked list) */
struct ext {
    char name[MAX_SYMBOL_LEN]; /* The name of the external symbol */
    unsigned int address; /* The address associated with the external symbol */
    Extptr next; /* Pointer to the next external symbol in the ext table */
};

/**
 * Creates output files based on the given source filename.
 *
 * @param source_filename The name of the source file.
 *
 * @remarks This function utilizes the global flag variables `is_entry_exists` and `is_extern_exists`.
 */
void create_output_files(char *source_filename) {
    /* Generate modified filename for object file */
    char *modified_filename_object = generate_new_filename(source_filename, FILE_OBJECT);

    /* Open the object file for writing */
    FILE *object_fd = fopen(modified_filename_object, "w");
    if (object_fd == NULL) {
        print_error(CANNOT_CREATE_FILE);
        free(modified_filename_object);
        exit(1);
    }

    /* Create .ob file */
    create_ob_file(object_fd);

    free(modified_filename_object);
    fclose(object_fd);

    /* Check if the flag for entry exists is true */
    if (is_entry_exists) {
        /* Generate modified filename for entries file */
        char *modified_filename_entries = generate_new_filename(source_filename, FILE_ENTRIES);

        /* Open the entries file for writing */
        FILE *entries_fd = fopen(modified_filename_entries, "w");
        if (entries_fd == NULL) {
            print_error(CANNOT_CREATE_FILE);
            free(modified_filename_entries);
            exit(1);
        }

        /* Create .ent file */
        create_ent_file(entries_fd);

        free(modified_filename_entries);
        fclose(entries_fd);
    }

    /* Check if the flag for extern exists is true */
    if (is_extern_exists) {
        /* Generate modified filename for externals file */
        char *modified_filename_externals = generate_new_filename(source_filename, FILE_EXTERNALS);

        /* Open the externals file for writing */
        FILE *externals_fd = fopen(modified_filename_externals, "w");
        if (externals_fd == NULL) {
            print_error(CANNOT_CREATE_FILE);
            free(modified_filename_externals);
            exit(1);
        }

        /* Create .ext file */
        create_ext_file(externals_fd);

        free(modified_filename_externals);
        fclose(externals_fd);
    }
}

/**
 * Creates the entries file, containing the names and addresses of symbols marked as entries.
 *
 * @param fd The file stream to write the entries file to.
 */
void create_ent_file(FILE *fd) {
    Symbolptr current_symbol = symbol_table;

    /* Iterate over the symbol table */
    while (current_symbol) {
        /* Check if the symbol is marked as an entry */
        if (current_symbol->is_ent) {
            /* Write the symbol's name and address to the file */
            fprintf(fd, "%s\t%d\n", current_symbol->name, current_symbol->address);
        }

        /* Move to the next symbol in the table */
        current_symbol = current_symbol->next;
    }
}

/**
 * Creates the externals file, containing the names and addresses of external symbols.
 *
 * @param fd The file stream to write the externals file to.
 *
 * @remarks This function utilizes the global variable 'ext_table'.
 */
void create_ext_file(FILE *fd) {
    Extptr current_ext = ext_table;

    /* Iterate over the external symbols table */
    while (current_ext) {
        /* Write the symbol's name and address to the file */
        fprintf(fd, "%s\t%d\n", current_ext->name, current_ext->address);

        /* Move to the next symbol in the table */
        current_ext = current_ext->next;
    }
}

/**
 * Creates the object file by writing the values of ic and dc, along with the encoded code and data segments, to the given file.
 *
 * @param fd The file descriptor of the object file.
 *
 * @remarks This function utilizes the global variables `ic`, `dc`, `code`, and `data`.
 */
void create_ob_file(FILE *fd) {
    int i;
    char *encoded_string;

    /* Write the values of ic and dc to the file */
    fprintf(fd, "%d\t%d\n", ic, dc);

    for (i = 0; i < ic + dc; i++) {
        if (i < ic) {
            /* Encode the segment of the code array at index i and assign the result to encoded_string */
            encoded_string = convert_to_base64(code[i]);
        } else {
            /* Encode the segment of the data array at index i - ic and assign the result to encoded_string */
            encoded_string = convert_to_base64(data[i - ic]);
        }

        /* Write the encoded string to the file */
        fprintf(fd, "%s\n", encoded_string);

        /* Free the memory allocated for encoded_string */
        free(encoded_string);
    }
}

/**
 * Converts the given word to a Base64 encoded string.
 *
 * @param word The unsigned integer to be converted.
 *
 * @return The Base64 encoded string representing the given word.
 *
 * @remarks This function utilizes the constant base64_table[].
 */
char *convert_to_base64(unsigned int word) {
    const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /* Allocate memory for the encoded string */
    char *encoded_string = (char *)malloc((BASE64_ENCODED_STRING_SIZE + 1) * sizeof(char));
    if (encoded_string == NULL) {
        print_error(MEM_ALLOC_FAILED);
        exit(1);
    }

    /* Extract the second half of the word and map it to the corresponding Base64 character */
    encoded_string[0] = base64_table[extract_bits(word, SECOND_HALF_START, SECOND_HALF_END)];

    /* Extract the first half of the word and map it to the corresponding Base64 character */
    encoded_string[1] = base64_table[extract_bits(word, FIRST_HALF_START, FIRST_HALF_END)];

    /* Set the null terminator at the end of the string */
    encoded_string[2] = '\0';

    return encoded_string;
}