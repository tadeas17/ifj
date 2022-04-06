/*  *********************
    lexical_analyzer.c
    Scanner module
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

#include <string.h>
#include <stdlib.h>
#include "lexical_analyzer.h"
#include "symtable.h"

#define MAX_TERM_LEN 256
#define RES_WORD_COUNT 9

htab_t* reserved_table;

int lex_analyzer_init()
{
    reserved_table = htab_init(RES_WORD_COUNT);
    if (reserved_table == NULL)
        return 0;
    int c = 0;
    c += htab_add(reserved_table,"else");
    c += htab_add(reserved_table,"float64");
    c += htab_add(reserved_table,"for");
    c += htab_add(reserved_table,"func");
    c += htab_add(reserved_table,"if");
    c += htab_add(reserved_table,"int");
    c += htab_add(reserved_table,"package");
    c += htab_add(reserved_table,"return");
    c += htab_add(reserved_table,"string");
    
    if(c == RES_WORD_COUNT)
        return 1;
    else
        return 0;
}

void lex_analyzer_close()
{
    htab_destroy(reserved_table);
}

void free_token(token_t* token)
{
    if(token!=NULL)
    {
        if(token->data != NULL)
            free(token->data);
        
        free(token);
    }
}

typedef enum state {
    end,
    begin,
    identifier_s,
    string_s,
    int_s,
    float_s,
    definition_s,
    assignment_s,
    comparator_s,
    not_equal_s
} state_t;

token_t* token_create(size_t data_len, token_type_t type, char* data)
{
    token_t* new_token = malloc(sizeof(token_t));
    new_token->type = type;
    new_token->data = calloc(data_len+1, sizeof(char));
    if (new_token->data == NULL)
        return NULL;

    strncpy(new_token->data, data, data_len);

    return new_token;
}

token_t* get_token()
{    
    state_t current_state = begin;
    int input;
    token_t token;
    char data[256];
    for( int i = 0; i<256; i++)
    {
        data[i] = 0;
    }
    token.data = data;

//tady obří switch, kterej vlastně reprezentuje konečnej automat (hraje si se současnym stavem)
    while (current_state != end)
    {    
        input = getchar();
        switch (current_state)
        {
            case begin:
                if( input == '(')
                {
                    token.type = L_brace;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == ')')
                {
                    token.type = R_brace;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == '!')
                {
                    token.type = logical_operator;
                    token.data[0] = input;
                    current_state = not_equal_s;
                    break;
                }
                else if (input ==  '\"')
                {
                    token.type = string_literal;
                    token.data[0] = input;
                    current_state = string_s;
                    break;
                }    
                else if (input == '.')
                {
                    token.type = float_literal;
                    token.data[0] = input;
                    current_state = float_s;
                    break;
                }
                else if (input == ',')
                {
                    token.type = comma;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == ';')
                {
                    token.type = delimiter;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == '>' || input == '<')
                {
                    token.type = logical_operator;
                    token.data[0] = input;
                    current_state = comparator_s;
                    break;
                }
                else if (input == '=')
                {
                    token.type = assignment;
                    token.data[0] = input;
                    current_state = comparator_s;
                    break;
                }
                
                else if (input >= '0' && input <= '9')
                {
                    token.type = int_literal;
                    token.data[0] = input;
                    current_state = int_s;
                    break;
                }
                else if ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z') || (input == '_'))
                {
                    token.type = identifier;
                    token.data[0] = input;
                    current_state = identifier_s;
                    break;
                }
                else if (input == '{')
                {
                    token.type = bloc_begin;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == '}')
                {
                    token.type = bloc_end;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == EOF)
                {
                    token.type = EOF_t;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == '\t' || input == ' ' )
                {
                    current_state = begin;
                    break;
                }
                else if (input == '\n')
                {
                    token.type = newline;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }    
                else if (input == '+' || input == '-' )  //takže je nutný další stav pro '-', kdy se rozhodne, jestli to je číslo nebo operátor?
                                                        //vlastně to může zůstat jen jako operátor, před číslem se to nemusí řešit
                {
                    token.type = add_sub_operator;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == '*' || input == '/')  //takže je nutný další stav pro '-', kdy se rozhodne, jestli to je číslo nebo operátor?
                {
                    token.type = mul_div_operator;
                    token.data[0] = input;
                    current_state = end;
                    break;
                }
                else if (input == ':')
                {
                    current_state = definition_s;
                    token.data[0] = input;
                    break;
                }
                
                break;
            case string_s:
                if (input == '\"')
                {
                    current_state = end;
                    token.data[strlen(token.data)] = input;
                    break;
                }
                else
                {
                    token.data[strlen(token.data)] = input;
                    break;
                }
            case int_s:
                if (input >= '0' && input <= '9')
                {
                    token.data[strlen(token.data)] = input;
                    break;
                }
                else if (input == '.')
                {
                    token.type = float_literal;
                    token.data[strlen(token.data)] = input;
                    break;
                }
                else
                {
                    current_state = end;
                    ungetc(input, stdin);
                    break;
                }
            case float_s:
                if (input >= '0' && input <= '9')
                {
                    token.data[strlen(token.data)] = input;
                    break;
                }
                else if (input == 'f')
                {   
                    token.data[strlen(token.data)] = input;
                    current_state = end;
                    break;
                }
                else
                {
                    current_state = end;
                    ungetc(input,stdin);
                    break;
                }                
            case identifier_s:
                if((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z') || input == '_')
                {
                    token.data[strlen(token.data)] = input;
                    break;
                }
                else
                {
                    current_state = end;
                    ungetc(input,stdin);
                    break;
                }
            case definition_s:
                if(input == '=')
                {
                    token.data[1] = input;
                    token.type = definition;
                    current_state = end;
                    break;
                }
                else
                {
                    token.type = colon;
                    ungetc(input, stdin);
                    current_state = end;
                    break;   
                }
            case not_equal_s:
                if(input == '=')
                {
                    token.data[1] = input;
                    token.type = assignment;
                    current_state = end;
                    break;
                }
                else
                {
                    return NULL;
                }
            case comparator_s:
                if (input == '=')
                {
                    token.data[1] = input;
                    token.type = logical_operator;
                    current_state = end;
                    break;
                }
                else
                {
                    ungetc(input, stdin);
                    current_state = end;
                    break;
                }
                
                
            default:
                current_state = end;
                break;
        }   
    }

    if (token.type == identifier)
    {
        if (htab_check(reserved_table, token.data))
        {
            token.type = reserved_word;
        }
        if(!strcmp(token.data, "string"))
            token.type = datatype;
        if(!strcmp(token.data, "int"))
            token.type = datatype;
        if(!strcmp(token.data, "float"))
            token.type = datatype;
    }
    
    return token_create(strlen(token.data), token.type, token.data);
}