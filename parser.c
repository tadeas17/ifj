/*  *********************
    parser.c
    Syntax Analyzer, Interface
    xbenes56, Dalibor Beneš
    xkotta00, Tadeáš Kot
    xsevci69, Jakub Ševčík
    VUT FIT, Brno, 2020
    ********************* */

    #include "parser.h"
    #include "lexical_analyzer.h"
    #include "support_types.h"

    #define getNewToken() if(token!=NULL) printf("token: %d %s\n", token->type, (token->data == NULL)? " " : token->data); if(token != NULL) free(token->data); free(token); token=get_token(); if (token==NULL) return 1; printf("new token: %d %s\n",token->type, token->data);
    #define tokenComp(t_type, t_data) ( token->type == t_type && (!strcmp (token->data, t_data)) )
    #define tokenDataComp(t_data) (!strcmp (token->data, t_data))
    
    #define TRUE 1
    #define FALSE 0

    #define checkRes(result) if(result == FALSE) return FALSE;

void astack_init(astack_t* stack)
{
    stack = malloc(sizeof(astack_t));
    if (stack == NULL)
        return;

    stack->top = NULL;
}

token_type_t astack_peek(astack_t* stack)
{
    if(stack->top != NULL)
        return stack->top->val;
    else
        return a_stack_error;
}

token_type_t astack_pop(astack_t* stack)
{
    token_type_t ret;
    if (ret = astack_peek(stack) != a_stack_error)
    {
        astack_item_t* tmp = stack->top;
        stack->top = stack->top->next_item;
        free(tmp);
    }
    return ret;
}

void astack_push(astack_t* stack, token_type_t item_val)
{
    astack_item_t* item = malloc(sizeof(astack_item_t));
    if (item == NULL)
        return;
    
    item->val = item_val;
    item->next_item = stack->top;
    stack->top = item;
}


    char a_table[7][7] = 
    {
        {'>', '<', '<', '<', '>', '<', '>' },   //log
        {'>', '>', '<', '<', '>', '<', '>' },   //add_sub
        {'>', '>', '>', '<', '>', '<', '>' },   //mul_div
        {'<', '<', '<', '<', '=', '<', 'e' },   //L_brace
        {'>', '>', '>', 'e', '>', 'e', '>' },   //R_brace
        {'>', '>', '>', 'e', '>', 'e', '>' },   //operand
        {'<', '<', '<', '<', 'e', '<', 'e' }    //end
    };

    token_t* token;

    int parse_multi_assign()
    {
        int ret = TRUE;
        
        if(token->type == assignment)
        {
            getNewToken();
            return TRUE;
        }
        
        if(token->type != comma)
            return FALSE;
        
        getNewToken();
        if(token->type != identifier)
            return FALSE;
        
        getNewToken();
        ret = parse_multi_assign();
        checkRes(ret);
        
        ret = parse_exp();
        checkRes(ret);
        
        if(token->type != comma)
            return FALSE;
        
        getNewToken();
        return ret;
    }

    int parse_can_else()
    {
        int ret = TRUE;
        
        if(token->type == newline)
        {
            getNewToken();
            return TRUE;
        }
        
        if(!tokenComp(reserved_word, "else"))
            return FALSE;
        
        getNewToken();
        if(token->type != bloc_begin)
            return FALSE;
        
        getNewToken();
        if(token->type != newline)
            return FALSE;
        
        getNewToken();
        ret = parse_stmnt_bloc();
        checkRes(ret);
        
        if(token->type != newline)
            return FALSE;
        
        getNewToken();
        if(token->type != bloc_end)
            return FALSE;
        
        getNewToken();
        ret = parse_can_else();
        
        return ret;
    }
// ty struktury a funkce pro ně asi někam dat jinam, nevim kam

    typedef enum rule_name {
            T0,
            T1,
            T2,
            T3,
            T4,
            T5,
            T6,
            T7,
            T8,
            T9,
            T10,
            T11,
            T12,
            T13,
            l_brace,
            r_brace,
            operand,
            log_t,
            add_t,
            mul_t,
            $_t
        } rule_name_t;

    typedef struct s_rule {
        struct s_rule *rule_next;
        rule_name_t rule_name;
        struct s_rule *rule_before;
    } s_rule_t;

    void free_s_rule(s_rule_t *rule) {
        while (rule) {
            free(rule);
            rule = rule->rule_before;
        }
    }


    s_rule_t *rule_create(rule_name_t rule_name) {
        s_rule_t *rule = malloc(sizeof(s_rule_t));
        if (!rule) {
            error_syntax(NULL);
            return NULL;
        }
        rule->rule_next = NULL;
        rule->rule_before = NULL;
        rule->rule_name = rule_name;
        return rule;
    }

    s_rule_t *rule_create_next(rule_name_t rule_name, s_rule_t *rule_actual) {
        s_rule_t *rule_next = rule_create(rule_name);
        rule_actual->rule_next = rule_next;
        rule_next->rule_before = rule_actual;
        return rule_next;
    }

    s_rule_t *rule_pop(s_rule_t * rule) {
        rule = rule->rule_before;
        free(rule->rule_next);
        return rule;
    }

    //todo: nejak sem nepochopil co delaj epsilon v te tabulce, tak to tady staci jen dopsat
    s_rule_t *rule_add(int rule_number, s_rule_t *rule) {
        switch (rule_number) {
            case 1:
                rule->rule_name = T2;
                return rule_create_next(T1, rule);;
                break;
            case 2:
                rule->rule_name = R_brace;
                rule = rule_create_next(T7, rule);
                return rule_create_next(L_brace, rule);
                break;
            case 3:
                rule->rule_name = operand;
                return rule;
                break;
            case 4:
                rule->rule_name = T2;
                rule = rule_create_next(T1, rule);
                return rule_create_next(log_t, rule);
                break;
            case 5: //epsilon
                break;
            case 6:
                rule->rule_name = T4;
                return rule_create_next(T3, rule);
                break;
            case 7:
                rule->rule_name = T4;
                rule = rule_create_next(T3, rule);
                return rule_create_next(add_t, rule);
                break;
            case 8: //epsilon
                break;
            case 9:
                rule->rule_name = T6;
                return rule_create_next(T5, rule);
                break;
            case 10:
                rule->rule_name = T6;
                rule = rule_create_next(T5, rule);
                return rule_create_next(mul_t, rule);
                break;
            case 11: //epsilon
                break;
            case 12:
                rule->rule_name = R_brace;
                rule = rule_create_next(T7, rule);
                return rule_create_next(L_brace, rule);
            case 13:
                rule->rule_name = T9;
                return rule_create_next(T8, rule);
                break;
            case 14:
                rule->rule_name = T9;
                rule = rule_create_next(T8, rule);
                return rule_create_next(log_t, rule);
                break;
            case 16:
                rule->rule_name = T11;
                return rule_create_next(T10, rule);
                break;
            case 17:
                rule->rule_name = T11;
                rule = rule_create_next(T10, rule);
                return rule_create_next(add_t, rule);
                break;
            case 19:
                rule->rule_name = T13;
                return rule_create_next(T12, rule);
                break;
        }
    }
    // něják sem to zatím nezkoušel

    token_type_t convert_to_astack()
    {
        if(token->type == add_sub_operator)
            return a_add_sub;
        if(token->type == mul_div_operator)
            return a_mul_div;
        if(token->type == logical_operator)
            return a_log;
    }

    //end symbol je token následující po EXP
    int parse_exp(/*token_type_t end_symbol*/) {
        token_type_t end_symbol;
        astack_t* stack;
        astack_init(stack);
        astack_push(stack, end_symbol);

        do
        {
                token_type_t a = astack_peek(stack);
                token_type_t b = token->type;

                switch (a_table[a - a_log][b - a_log])
                {
                case '=':
                    astack_push(stack, convert_to_astack());
                    getNewToken();
                    break;
                case '<':
                    
                
                default:
                    //destroy astack()
                    return FALSE;
                }

        }
        while (token->type != end_symbol && astack_peek(stack) != end_symbol);
 
        
    
    }

    int parse_var_stmnt()
    {
        int ret = FALSE;
        
        if(token->type == comma)
        {
            getNewToken();
            if(token->type != identifier)
                return FALSE;

            getNewToken();
            ret = parse_multi_assign();
            checkRes(ret);

            ret = parse_exp();
            checkRes(ret);

            if(token->type != comma)
                return FALSE;

            getNewToken();
            ret = parse_exp();
            checkRes(ret);
        }
        
        else if(token->type == definition)
        {
            getNewToken();
            ret = parse_exp();
            checkRes(ret);
        }
        
        else if(token->type == assignment)
        {
            getNewToken();
            ret = parse_exp();
            checkRes(ret);
        }
        
        else if(tokenComp(reserved_word, "return"))
        {
            getNewToken();
            ret = parse_exp();
            checkRes(ret);
        }
        
        return ret;
    }

    int parse_can_eol()
    {
        if(token->type == newline)
        {
            getNewToken();
        }
        return TRUE;
    }

    int parse_stmnt()
    {
        int ret = FALSE;
        
        if(token->type == identifier)
        {
            getNewToken();
            ret = parse_var_stmnt();
            checkRes(ret);

            if(token->type != newline)
                return FALSE;
            
            getNewToken();
        }
        
        else if(tokenComp(reserved_word, "if"))
        {
            getNewToken();
            ret = parse_exp();
            checkRes(ret);

            if(token->type != bloc_begin)
                return FALSE;

            getNewToken();
            if(token->type != newline)
                return FALSE;

            getNewToken();
            ret = parse_stmnt_bloc();
            checkRes(ret);

            if(token->type != newline)
                return FALSE;

            getNewToken();
            if(token->type != bloc_end)
                return FALSE;
            
            getNewToken();
            ret = parse_can_else();
            checkRes(ret);
        }
        
        else if(tokenComp(reserved_word, "for"))
        {
            getNewToken();
            if(token->type != identifier)
                return FALSE;

            getNewToken();
            if(token->type != definition)
                return FALSE;

            getNewToken();
            ret = parse_exp();
            checkRes(ret);

            if(token->type != delimiter)
                return FALSE;

            getNewToken();
            ret = parse_exp();
            checkRes(ret);

            if(token->type != delimiter)
                return FALSE;

            getNewToken();
            if(token->type != identifier)
                return FALSE;

            getNewToken();
            if(token->type != assignment)
                return FALSE;

            getNewToken();
            ret = parse_exp();
            checkRes(ret);

            if(token->type != bloc_begin)
                return FALSE;

            getNewToken();
            if(token->type != newline)
                return FALSE;

            getNewToken();
            ret = parse_stmnt_bloc();
            checkRes(ret);

            if(token->type != newline)
                return FALSE;

            getNewToken();
            if(token->type != bloc_end)
                return FALSE;

            getNewToken();
            if(token->type != newline)
                return FALSE;
            
            getNewToken();
        }
        
        return ret;
    }

    int parse_next_rettype()
    {
        int ret = TRUE;
        
        if(token->type == R_brace)
        {
            return TRUE;
        }
        
        if(token->type != comma)
            return FALSE;
        
        getNewToken();
        if(token->type != datatype)
            return FALSE;
        
        getNewToken();
        ret = parse_next_rettype();
        return ret;
    }

    int parse_next_param()
    {
        int ret = TRUE;
        
        printf("začátek next_param\ntoken: %d %s\n", token->type, token->data);
        if(token->type == R_brace)
        {
            printf("posledni param\n");
            //R_brace je potreba v parse_prog
            return TRUE;
        }
        
        ret = parse_can_eol();
        checkRes(ret);
        
        //getNewToken();
        if(token->type != comma)
            return FALSE;
        
        getNewToken();
        if(token->type != identifier)
            return FALSE;
        
        getNewToken();
        if(token->type != datatype && !tokenComp(reserved_word, "string") && !tokenComp(reserved_word, "int") && !tokenComp(reserved_word,"float"))
            return FALSE;
        
        printf("konec next param\n");
        getNewToken();
        ret = parse_next_param();
        return ret;
    }

    int parse_stmnt_bloc()
    {
        int ret = TRUE;
        
        if(token->type == bloc_end)
        {
            return TRUE;
        }
        
        ret = parse_stmnt();
        checkRes(ret);
        
        getNewToken();
        ret = parse_stmnt_bloc();
        checkRes(ret);
        
        getNewToken();
        return ret;
    }

    int parse_rettypes()
    {
        int ret = TRUE;
        
        if(token->type == bloc_begin)
        {
            getNewToken();
            return TRUE;
        }
        
        if(token->type != L_brace)
            return FALSE;
        
        getNewToken();
        printf("before datatype\n");
        printf("%d %s\n", token->type, token->data);
        if(tokenComp(reserved_word, "string"))
            printf("okokok\n");
        if(token->type != datatype && !tokenComp(reserved_word, "string") && !tokenComp(reserved_word, "int"))
            return FALSE;
        printf("after datatype\n");

        getNewToken();
        ret = parse_next_rettype();
        checkRes(ret);
        
        if(token->type != R_brace)
            return FALSE;
        
        getNewToken();
        return ret;
    }

    int parse_params()
    {
        int ret = TRUE;
        
        if(token->type == R_brace)
        {
            getNewToken();
            return TRUE;
        }
        
        if(token->type != identifier)
            return FALSE;
        
        getNewToken();
        if(token->type != datatype && !tokenComp(reserved_word, "string") && !tokenComp(reserved_word, "int") && !tokenComp(reserved_word, "float64"))
            return FALSE;
        
        getNewToken();
        ret = parse_next_param();
        
        //getNewToken();
        return ret;
    }

    int parse_prog()
    {
        int ret = TRUE;

        //pokud typ je EOF, pak je vsechno v pohode
        if(token->type == EOF_t)
            return TRUE;

        while(token->type == newline)
        {
            getNewToken();
        }
        //jinak sjedu cely radek tabulky
        if(!tokenComp(reserved_word, "func"))
            return FALSE;
        
        getNewToken();
        if(token->type != identifier)
            return FALSE;
        
        getNewToken();
        if(token->type != L_brace)
            return FALSE;

        getNewToken();
        ret = parse_params();
        checkRes(ret);

        if(token->type != R_brace)
            return FALSE;

        getNewToken();
        ret = parse_rettypes();
        checkRes(ret);

        if(token->type != bloc_begin)
            return FALSE;
        
        getNewToken();
        if(token->type != newline)
            return FALSE;
        
        getNewToken();
        ret = parse_stmnt_bloc();
        checkRes(ret);

        if(token->type != bloc_end)
            return FALSE;
        
        getNewToken();
        if(token->type == EOF_t)
            return TRUE;

        if(token->type != newline)
            return FALSE;
        
        //pokud volam funkci, tak musi vzdy uz mit predem pripraveny token!
        getNewToken();
        ret = parse_prog();
        return ret;
    }

    int parse_start()
    {
        if(token->type == EOF_t)
            return TRUE;

        if(!(tokenComp(reserved_word, "package")))
            return FALSE;
        
        printf("ok\n");
        getNewToken();
        printf("%d %s\n", token->type, token->data);    
        if(!(tokenComp(identifier, "main")))
            return FALSE;
        
        getNewToken();
        return parse_prog();
    }

    int prog()
    {
     //   token_t* token;
        getNewToken();

        int ret = FALSE;
        ret = parse_start();
        return ret;
    }

    int main()
    {
        lex_analyzer_init();
        if(prog())
            printf("syntax ok!\n");
        else
            printf("not ok!\n");

        lex_analyzer_close();
        return 0;
    }
