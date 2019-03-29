#include <stdio.h>
#include "y.tab.h"
#include "instruction.h"
#include "asm_parse.h"
#include <string.h>

extern FILE *asm_file;
extern list_t *var_list;
extern list_t *reg_list;

struct reg *operation(int op, struct reg *fst, struct reg *sec)
{ 
    if (fst->is_wrap) unwrap_register(fst, COPY_UNWR);
    if (sec->is_wrap) unwrap_register(sec, COPY_UNWR);

    return execute(op, fst, sec);
}

struct reg *unar_operation(int op, struct reg *fst)
{
    if (fst->is_wrap)
        unwrap_register(fst, COPY_UNWR);

    switch (op)
    {
        case '!':
        {
            struct reg *num = wrap_number(0);
            unwrap_register(num, COPY_UNWR);
            return execute(_eq, fst, num);
        }
        case '~':
        {
            struct reg *num = wrap_number(0xff);
            unwrap_register(num, COPY_UNWR);
            return execute('^', fst, num);
        }
        case '-':
        {
            struct reg *num = wrap_number(-1);
            unwrap_register(num, COPY_UNWR);
            return execute('*', fst, num);
        } 
        default:
            break;
    }
}

struct reg *assignment(struct reg *lvalue, int as_op, struct reg *statement)
{
    struct var *var_ptr = (struct var*)lvalue->value;

    if (!var_ptr->is_exists)
    {
        var_ptr->is_exists = 1;
        list_rpush(var_list, list_node_new((void *)var_ptr));

        fprintf(asm_file, "    str %s\n", var_ptr->name);
    }

    if (lvalue->is_wrap)
        unwrap_register(lvalue, ADDR_UNWR);
    if (statement->is_wrap)
        unwrap_register(statement, COPY_UNWR);
    
    switch (as_op)
    {
        case '=':
            print_instruction("mov", lvalue, statement);
            release_register(&statement);
            break;
        case _asum:
            execute('+', lvalue, statement);
            break;
        case _asub:
            execute('-', lvalue, statement);
            break;
        case _amul:
            execute('*', lvalue, statement);
            break;
        case _adiv:
            execute('/', lvalue, statement);
            break;
        case _amod:
            execute('%', lvalue, statement);
            break;
        case _axor:
            execute('^', lvalue, statement);
            break;
        case _aor:
            execute('|', lvalue, statement);
            break;
        case _aand:
            execute('&', lvalue, statement);
            break;
        case _alshift:
            execute(_lshift, lvalue, statement);
            break;
        case _arshift:
            execute(_rshift, lvalue, statement);
            break;
        default:
            release_register(&statement);
            printf("# No assignment found, %d(%c).\n", as_op, as_op);
            break;
    }
    
    release_register(&lvalue);
    
    return wrap_variable(var_ptr);
}

struct reg *increment(struct reg *reg)
{
    if (reg->is_wrap)
        unwrap_register(reg, ADDR_UNWR);

    struct var *var_ptr = (struct var *)reg->value;

    fprintf(asm_file, "    inc [%s]\n", reg->name);

    release_register(&reg);

    return wrap_variable(var_ptr);
}

struct reg *decrement(struct reg *reg)
{
    if (reg->is_wrap)
        unwrap_register(reg, ADDR_UNWR);

    struct var *var_ptr = (struct var*)reg->value;
 
    fprintf(asm_file, "    dec [%s]\n", reg->name);

    release_register(&reg);

    return wrap_variable(var_ptr);
}