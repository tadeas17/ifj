/*  *********************
    errors.c
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

#include "errors.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void error_alloc_null(char * error) { //Error po nepovedené alokaci paměti, pokud se parametr error zadá NULL vypíše se defaultní hláška
    if (error) {
        fpritnf(stderr, error);
        return;
    }
    fprintf(stderr, "Nedostatek paměti. (nepovedená alokace paměti)"); //TODO: Vymyslet lepší chybovou hlášku :D
}

void error_syntax(char * error) {
    if (error) {
        fpritnf(stderr, error);
        exit(2);
        return;
    }
    fprintf(stderr, "Synatkticka chyba.");
    exit(2);
}