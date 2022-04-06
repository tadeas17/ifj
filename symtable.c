/*  *********************
    symtable.c
    String Hash table
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

#include "symtable.h"

size_t hash_fun(char *string)
{
    size_t hashval = 0;

    while (*string)
    {
        hashval += *string + 31 * hashval;
        string++;
    }

    return hashval;
}

size_t htab_get_index(htab_t* hash_table, char* string)
{
    return hash_fun(string) % hash_table->bucket_size;
}

htab_t* htab_init(size_t size)
{
    htab_t* hash_table = malloc(sizeof(htab_t));
    if (hash_table == NULL)
        return NULL;

    hash_table->bucket_size = size;

    hash_table->list = malloc(sizeof(htab_elem_t) * size);
    if (hash_table->list == NULL)
    {
        free(hash_table);
        return NULL;
    }


    for (int i = 0; i<size; i++)
    {
        (hash_table->list)[i].data = NULL;
        (hash_table->list)[i].next = NULL;
    }
    
    return hash_table;
}

int htab_check(htab_t* hash_table, char* string)
{
    htab_elem_t* iterator = &(hash_table->list[htab_get_index(hash_table, string)]);

    while (iterator->next != NULL)
    {
        if (strcmp(iterator->next->data,string) == 0)
            return 1;
 
        iterator = iterator->next;
    }

    return 0;
}

int htab_add(htab_t* hash_table, char* string)
{
    if (htab_check(hash_table, string))
    {
        return 2;
    }
    
    htab_elem_t* iterator = &(hash_table->list[htab_get_index(hash_table, string)]);
    while(iterator->next != NULL)
        iterator = iterator->next;

    iterator->next = malloc(sizeof(htab_elem_t));
    if (iterator->next == NULL)
        return 0;

    iterator->next->next = NULL;
    iterator->next->data = calloc(strlen(string) + 1,sizeof(char));

    if (iterator->next->data == NULL)
    {
        free(iterator->next);
        iterator->next = NULL;
        return 0;
    }

    strncpy(iterator->next->data, string, strlen(string));

    return 1;
}

int htab_remove(htab_t* hash_table, char* string)
{
    if(htab_check(hash_table, string))
    {
        htab_elem_t* iterator = &(hash_table->list[htab_get_index(hash_table, string)]);
        while(strcmp(iterator->next->data, string) != 0)
            iterator = iterator->next;

        htab_elem_t* tmp = iterator->next;
        iterator->next = iterator->next->next;

        free(tmp->data);
        free(tmp);
        
        return 1;
    }
    else
    {
        return 2;
    }   
}

int htab_destroy(htab_t* hash_table)
{
    for(int i = 0; i < hash_table->bucket_size; i++)
    {
        while(hash_table->list[i].next != NULL)
        {
            htab_elem_t* tmp = hash_table->list[i].next;
            hash_table->list[i].next = tmp->next;

            free(tmp->data);
            free(tmp);
        }
    }

    free(hash_table->list);
    free(hash_table);
}