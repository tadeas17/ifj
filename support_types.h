/*  *********************
    support_types.h
    Data types
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */
#include <stdlib.h>

#ifndef SUPP_TYPES_H
#define SUPP_TYPES_H

typedef enum token_type {
    identifier,         // my_var
    datatype,           // int
    reserved_word,      // if
    int_literal,        // 1234
    string_literal,     // "string"
    float_literal,      // 0.0f
    L_brace,            // (
    R_brace,            // )
    bloc_begin,         // {
    bloc_end,           // }
    add_sub_operator,   //operátor + -
    mul_div_operator,   // operátor * /
    logical_operator,   // operátory >= <= > < ==
    assignment,         // =
    EOF_t,              // EOF
    comma,              // ,
    delimiter,          // ;
    definition,         // :=
    colon,              // :
    newline,            // \n

    //arith_type
    a_log,          // > , < , >= , <= , == , != 
    a_add_sub,      // + , -
    a_mul_div,      // * , /
    a_L_brace,      // (
    a_R_brace,      // )
    a_operand,      // literal, identifier
    a_end,          // $?

    a_next,         // "<"

    a_rule_1        // E -> i
    a_rule_2        // E -> E log E
    a_rule_3        // E -> E add_sub E
    a_rule_4        // E -> E mul_div E
    a_rule_5        // E -> ( E )

    a_stack_error

} token_type_t;


typedef struct token {
    token_type_t type;
    char* data;
} token_t;

typedef struct arith_stack_item astack_item_t;


struct arith_stack_item
{
    token_type_t val;
    astack_item_t* next_item;
};

typedef struct arith_stack
{
    astack_item_t* top;
} astack_t;


#endif
/*typedef struct expresion {
    struct expresion *expresion_left;
    token_t *token;
    struct expresion *expresion_right;
} expresion_t;

typedef struct stuck {
    expresion_t *start_stuck;
    expresion_t *end_stuck;
} stuck_t;
*/