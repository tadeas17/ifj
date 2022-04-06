/*  *********************
    parser.h
    Syntax analyzer, Interface
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

int prog();