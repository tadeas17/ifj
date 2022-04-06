/*  *********************
    lexical_analyzer.h
    Scanner module, Interface
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

#include "support_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t* get_token();

void free_token();

int lex_analyzer_init();

void lex_analyzer_close();