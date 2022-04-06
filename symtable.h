/*  *********************
    symtable.h
    String Hash table, Interface
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct htab_elem htab_elem_t;

struct htab_elem {
    char* data;
    htab_elem_t* next;
};

typedef struct htab {
    size_t bucket_size;
    htab_elem_t* list;
} htab_t;

htab_t* htab_init(size_t size);

int htab_check(htab_t* hash_table, char* string);

int htab_add(htab_t* hash_table, char* string);

int htab_remove(htab_t* hash_table, char* string);

int htab_destroy(htab_t* hash_table);

