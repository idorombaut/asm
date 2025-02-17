/**
 * This file contains the implementation of various functions related to symbol management in
 * an assembly language program. It defines structures for symbols and external symbols, as
 * well as functions for updating symbol addresses, creating symbols, adding symbols to the
 * symbol table, deleting symbols, and managing external symbols.
 */

#include <stdlib.h>
#include <string.h>
#include "symbol_structs.h"
#include "utils.h"
#include "globals.h"

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
 * Updates the addresses of symbols in the symbol table.
 *
 * @param table       The symbol table.
 * @param addr_inc    The value to increment the addresses of symbols by.
 * @param type        The type of the statement (data, instruction).
 */
void update_symbol_addr(Symbolptr table, unsigned int addr_inc, statement_type type) {
    Symbolptr current_symbol = table;

    /* Iterate through each symbol in the symbol table */
    while (current_symbol != NULL) {
        /* Check if the symbol is not external and matches the desired statement type */
        if (!current_symbol->is_ext && current_symbol->type == type) {
            /* Update the symbol's address by incrementing it with addr_inc */
            current_symbol->address += addr_inc;
        }

        /* Move to the next symbol in the table */
        current_symbol = current_symbol->next;
    }
}

/**
 * Makes a symbol an entry symbol.
 *
 * @param head The head of the symbol table.
 * @param name The name of the symbol to make an entry.
 *
 * @return A boolean indicating whether making the symbol an entry was successful.
 *
 * @remarks This function utilizes the global flag is_entry_exists to keep track of whether an entry symbol exists or not.
 *          The flag is updated when a symbol is successfully made an entry.
 */
boolean make_entry(Symbolptr head, char *name) {
    /* Find the symbol in the symbol table */
    Symbolptr symbol = find_symbol(head, name);

    if (symbol != NULL) {
        if (symbol->is_ext) {
            /* The symbol is an external symbol, it cannot be made an entry */
            print_error(ENTRY_CANNOT_BE_EXTERN);
            return FALSE;
        }

        /* Make the symbol an entry symbol */
        symbol->is_ent = TRUE;

        /* Update the global flag to indicate an entry symbol exists */
        is_entry_exists = TRUE;
    } else {
        /* Symbol not found in the symbol table */
        print_error(ENTRY_SYMBOL_NOT_FOUND);
        return FALSE;
    }

    return TRUE;
}

/**
 * Retrieves the address associated with a symbol.
 *
 * @param head The head of the symbol table.
 * @param name The name of the symbol to retrieve the address for.
 *
 * @return Returns the address of the symbol if found, or NONE_ADDR if the symbol is not found.
 */
unsigned int get_symbol_addr(Symbolptr head, char *name) {
    /* Find the symbol in the symbol table */
    Symbolptr symbol = find_symbol(head, name);

    /* If the symbol is found, return its address; otherwise, return NONE_ADDR */
    return (symbol != NULL) ? symbol->address : NONE_ADDR;
}

/**
 * Checks if a symbol is an external symbol.
 *
 * @param head The head of the symbol table.
 * @param name The name of the symbol to check.
 *
 * @return Returns TRUE if the symbol is found and is an external symbol, FALSE otherwise.
 */
boolean is_extern_symbol(Symbolptr head, char *name) {
    /* Find the symbol in the symbol table */
    Symbolptr symbol = find_symbol(head, name);

    /* If the symbol is found, return its is_ext flag; otherwise, return FALSE */
    return (symbol != NULL) ? symbol->is_ext : FALSE;
}

/**
 * Checks if a symbol exists in the symbol table.
 *
 * @param head The head of the symbol table.
 * @param name The name of the symbol to check.
 *
 * @return Returns TRUE if the symbol exists in the symbol table, FALSE otherwise.
 */
boolean is_existing_symbol(Symbolptr head, char *name) {
    /* Find the symbol in the symbol table */
    Symbolptr symbol = find_symbol(head, name);

    /* Return TRUE if the symbol is found, FALSE otherwise */
    return symbol != NULL;
}

/**
 * Finds a symbol in the symbol table by name.
 *
 * @param table The symbol table to search in.
 * @param name  The name of the symbol to find.
 *
 * @return Returns a pointer to the symbol if found, or NULL if the symbol is not found.
 */
Symbolptr find_symbol(Symbolptr table, char *name) {
    /* Start from the head of the symbol table */
    Symbolptr current_symbol = table;

    /* Iterate through the symbol table until the end is reached */
    while (current_symbol) {
        /* Compare the name of the current symbol with the given name */
        if (strcmp(current_symbol->name, name) == 0) {
            /* The symbol is found, return its pointer */
            return current_symbol;
        }

        /* Move to the next symbol in the symbol table */
        current_symbol = current_symbol->next;
    }

    /* The symbol is not found in the symbol table */
    return NULL;
}

/**
 * Creates a new symbol with the given properties.
 *
 * @param name      The name of the symbol.
 * @param address   The address associated with the symbol.
 * @param is_ext    Indicates if the symbol is an external symbol.
 *
 * @return Returns a pointer to the created symbol.
 *
 * @remarks This function utilizes the global flag is_extern_exists to keep track of whether an external symbol exists or not.
 *          The flag is updated when an external symbol is created.
 */
Symbolptr create_symbol(char *name, unsigned int address, boolean is_ext) {
    Symbolptr symbol = (Symbolptr)malloc(sizeof(Symbol));
    if (symbol == NULL) {
        print_error(MEM_ALLOC_FAILED);
        exit(1);
    }

    strcpy(symbol->name, name);
    symbol->address = address;
    symbol->is_ext = is_ext;
    symbol->is_ent = FALSE; /* Temporary exclusion of .entry directive consideration */
    symbol->next = NULL;

    if (is_ext) {
        symbol->type = DIRECTIVE;
        is_extern_exists = TRUE; /* Update the global flag to indicate an external symbol exists */
    }

    return symbol;
}

/**
 * Adds a symbol to the symbol table.
 *
 * @param head      The head of the symbol table.
 * @param name      The name of the symbol.
 * @param address   The address associated with the symbol.
 * @param is_ext    Indicates if the symbol is an external symbol.
 *
 * @return Returns a pointer to the added symbol if successful, or NULL if the symbol already exists.
 */
Symbolptr add_symbol_to_list(Symbolptr *head, char *name, unsigned int address, boolean is_ext) {
    Symbolptr new_symbol;

    /* Check if the symbol already exists in the symbol table */
    if (is_existing_symbol(*head, name)) {
        print_error(SYMBOL_ALREADY_EXISTS);
        return NULL;
    }

    /* Create a new symbol with the given properties */
    new_symbol = create_symbol(name, address, is_ext);

    if (*head == NULL) {
        /* If the symbol table is empty, set the new symbol as the head */
        *head = new_symbol;
    } else {
        /* Find the last symbol in the symbol table */
        Symbolptr current = *head;
        while (current->next != NULL) {
            current = current->next;
        }

        /* Append the new symbol to the end of the symbol table */
        current->next = new_symbol;
    }

    return new_symbol;
}

/**
 * Frees the memory occupied by the symbol table.
 *
 * @param head A pointer to the head pointer of the symbol table.
 */
void free_symbol(Symbolptr *head) {
    Symbolptr current;
    Symbolptr next;

    /* Nothing to free if the symbol table is empty */
    if (*head == NULL) {
        return;
    }

    current = *head;

    /* Traverse the symbol table and free each symbol */
    while (current != NULL) {
        /* Store the next symbol before freeing the current symbol */
        next = current->next;

        /* Free the memory occupied by the current symbol */
        free(current);

        /* Move to the next symbol */
        current = next;
    }

    /* Set the head pointer to NULL to indicate an empty symbol table */
    *head = NULL;
}

/**
 * Deletes a symbol from the symbol table.
 *
 * @param head The head of the symbol table.
 * @param name The name of the symbol to delete.
 */
void delete_symbol(Symbolptr *head, char *name) {
    /* Find the symbol to delete */
    Symbolptr symbol_to_delete = find_symbol(*head, name);

    if (symbol_to_delete != NULL) {
        Symbolptr current = *head;
        Symbolptr previous = NULL;

        /* Traverse the symbol table to find the symbol to delete and its previous symbol */
        while (current != symbol_to_delete) {
            previous = current;
            current = current->next;
        }

        if (previous == NULL) {
            /* Symbol to delete is the first symbol in the list */
            *head = symbol_to_delete->next;
        } else {
            /* Symbol to delete is not the first symbol */
            previous->next = symbol_to_delete->next;
        }

        free(symbol_to_delete); /* Free the memory occupied by the symbol */
    }
}

/**
 * Creates a new external symbol and initializes its properties.
 *
 * @param name      The name of the external symbol.
 * @param address   The address associated with the external symbol.
 *
 * @return A pointer to the newly created external symbol.
 */
Extptr create_ext(char *name, unsigned int address) {
    Extptr ext = (Extptr)malloc(sizeof(Ext));
    if (ext == NULL) {
        print_error(MEM_ALLOC_FAILED);
        exit(1);
    }

    strcpy(ext->name, name);
    ext->address = address;
    ext->next = NULL;

    return ext;
}

/**
 * Adds a new external symbol to the end of the linked list.
 *
 * @param head      A pointer to the head of the linked list.
 * @param name      The name of the external symbol.
 * @param address   The address associated with the external symbol.
 *
 * @return A pointer to the newly added external symbol.
 */
Extptr add_ext_to_list(Extptr *head, char *name, unsigned int address) {
    /* Create a new external symbol with the given name and address */
    Extptr new_ext = create_ext(name, address);

    if (*head == NULL) {
        /* If the list is empty, make the new symbol the head */
        *head = new_ext;
    } else {
        /* Traverse the list to find the last symbol */
        Extptr current = *head;
        while (current->next != NULL) {
            current = current->next;
        }

        /* Add the new symbol to the end of the list */
        current->next = new_ext;
    }

    return new_ext;
}

/**
 * Frees the memory occupied by the linked list of external symbols.
 *
 * @param head A pointer to the head pointer of the linked list.
 */
void free_ext(Extptr *head) {
    Extptr current;
    Extptr next;

    /* If the list is empty or head is NULL, nothing to free, so return */
    if (*head == NULL) {
        return;
    }

    current = *head;

    /* Traverse the linked list and free each node */
    while (current != NULL) {
        /* Store the next pointer before freeing the current node */
        next = current->next;

        /* Free the memory occupied by the current node */
        free(current);

        /* Move to the next node */
        current = next;
    }

    /* Set the head pointer to NULL to indicate an empty list */
    *head = NULL;
}