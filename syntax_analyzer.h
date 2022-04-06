/*  *********************
    syntax_analyzer.h
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

#include "support_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical_analyzer.h"
#include "errors.h"
#include <stdbool.h>
#include "symtable.h"

expresion_t *expresion_create();

void expresion_insert_left();

void expresion_insert_right();

void expresion_create_left();

void expresion_create_right();

stuck_t *create_stuck();

void expresion_create_end();

expresion_t *stuck_pop_end();

void syntax_analyzer();