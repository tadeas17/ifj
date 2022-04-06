/*  *********************
    syntax_analyzer.c
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

#include <string.h>
#include <stdlib.h>
#include "syntax_analyzer.h"
#include <stdio.h>
#include <stdbool.h>

htab_t *identifier_table;

//muze byt pred daty bloc_begin?
// pred tokenem muze byt L_brace R_brace bloc_begin bloc_end arith_operator logical_operator newline definition delimiter
#define can_be_number(token_type) \
    (token_type == L_brace || token_type == bloc_begin || token_type == arith_operator || token_type == logical_operator\
    || token_type == newline || token_type == definition || token_type == delimiter || token_type == assignment || token_type == comma)

#define can_be_operator(token_type) \
    (token_type == L_brace || token_type == bloc_begin || token_type == bloc_end || token_type == R_brace || \
    token_type == int_literal || token_type == float_literal || token_type == arith_operator)


typedef enum block_state {
    block_s,
    condition_s,
    function_s,
    for_s,
    while_s,
    NONE_s
} block_state_t;


expresion_t *expresion_create(token_t *token) { //vytvory expresion_t
    expresion_t *expresion = malloc(sizeof(expresion_t));
    if (!expresion) {
        error_alloc_null(NULL);
        return NULL;
    }
    expresion->token = token;
    expresion->expresion_left = NULL;
    expresion->expresion_right = NULL;
    return expresion;
}

void expresion_insert_left(expresion_t *expresion, expresion_t *expresion_left,
                           stuck_t *stuck) { //vlozi expresion_left nalevo
    expresion_left->expresion_right = expresion;
    expresion_left->expresion_left = expresion->expresion_left;
    if (expresion->expresion_left) {
        expresion->expresion_left->expresion_right = expresion_left;

    } else {
        stuck->start_stuck = expresion_left;
    }
    expresion->expresion_left = expresion_left;
}

void expresion_insert_right(expresion_t *expresion, expresion_t *expresion_right,
                            stuck_t *stuck) { //vlozi expresion_right doprava
    expresion_right->expresion_left = expresion;
    expresion_right->expresion_right = expresion->expresion_right;
    if (expresion->expresion_right) {
        expresion->expresion_right->expresion_left = expresion_right;

    } else {
        stuck->end_stuck = expresion;
    }
    expresion->expresion_right = expresion_right;
}

void expresion_create_left(expresion_t *expresion, token_t *token, stuck_t *stuck) {
    expresion_t *expresion_left = expresion_create(token);
    expresion_insert_left(expresion, expresion_left, stuck);
}

void expresion_create_right(expresion_t *expresion, token_t *token, stuck_t *stuck) {
    expresion_t *expresion_right = expresion_create(token);
    expresion_insert_right(expresion, expresion_right, stuck);
}


stuck_t *create_stuck(expresion_t *start_stuck, expresion_t *end_stuck) {
    stuck_t *stuck = malloc(sizeof(stuck_t));
    if (!stuck) {
        error_alloc_null(NULL);
    }
    stuck->start_stuck = start_stuck;
    stuck->end_stuck = end_stuck;
    return stuck;
}

void expresion_create_end(token_t *token, stuck_t * stuck) {
    expresion_t *expresion = expresion_create(token);
    expresion_insert_right(stuck->end_stuck, expresion, stuck);
}

expresion_t *stuck_pop_end(stuck_t *stuck) { //sracka upravit toto
    if (stuck->end_stuck) {
        expresion_t *expresion_end = stuck->end_stuck;
        stuck->end_stuck = expresion_end->expresion_left;
        return expresion_end;
    }
    return NULL;
}






void syntax_analyzer() {
    printf("aaa\n");
    token_t *token = get_token();
    stuck_t *stuck = create_stuck(token, token);
    token_type_t before_token_type = newline;
    block_state_t state = NONE_s;
    while (token) {


        switch (token->type) {
            case identifier:

                break;
            case datatype:
                break;
            case reserved_word:
                break;
            case int_literal:
                if (!can_be_now_number(stuck->end_stuck->token->type)) {
                    //todo: uvolnit pamet
                    error_syntax();

                }
                expresion_create_end(token, stuck);
                break;
            case string_literal:
                break;
            case float_literal:
                if (!can_be_now_number(stuck->end_stuck->token->type)) {
                    //todo: uvolnit pamet
                    error_syntax();

                }
                expresion_create_end(token, stuck);
                break;
            case L_brace:
                if (!(stuck->end_stuck->token->type==reserved_word || can_be_now_number()))
                break;
            case R_brace:
                break;
            case bloc_begin:
                break;
            case bloc_end:
                break;
            case arith_operator:
                break;
            case logical_operator:
                break;
            case assignment:
                if (stuck->end_stuck->token->type != identifier) {
                    error_syntax();;
                    //todo uvolnit pamet
                }
                for (expresion_t *expresion = stuck->end_stuck; expresion->token->type != newline;) {
                    if (expresion->token->type == comma || htab_check(identifier_table, expresion->token->data) || expresion->token->data == "_") { //pokud to je , nebo je definovana nebo _
                        expresion = expresion->expresion_left;
                    }
                    else {
                        error_syntax(NULL);
                    }
                }

                break;
            case EOF_t:
                break;
            case comma:
                break;
            case delimiter:
                break;
            case definition:
                if (stuck->end_stuck->token->type != identifier) {
                    error_syntax(NULL);
                }
                for (expresion_t *expresion = stuck->end_stuck; expresion->token->type != newline) {
                    if (expresion->token->type == comma || !htab_check(identifier_table, expresion) || expresion->token->data == "_") { //pokud to je , nebo neni definovana nebo _
                        expresion = expresion->expresion_left;
                    }
                    else {
                        error_syntax(NULL);
                    }
                }

                break;
            case colon:
                break;
            case newline:
                break;

        }
        token = get_token();

        expresion_create_end(token, stuck);
    }
    for (int i =0; i < 50; i++) {
        char *dat = stuck_pop_end(stuck)->token->data;
        printf("%s", dat);
    }

}






