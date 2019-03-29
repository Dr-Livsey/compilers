#include <stdio.h>
#include "instruction.h"
#include "asm_parse.h"

extern list_t *reg_list;
extern FILE *asm_file;


//labels
enum {JL, JG, JLE, JGE, EQ, NEQ, MEQ, LEQ};
unsigned lables_cnt[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

struct reg *execute(int op, struct reg *lval, struct reg *rval)
{
    switch (op)
    {
    case '+':
        print_instruction("add", lval, rval);
        break;
    case '-':
        print_instruction("sub", lval, rval);
        break;
    case '*':
        print_instruction("mul", lval, rval);
        break;
    case '/':
        print_instruction("div", lval, rval);
        break;
    case '%':
        print_instruction("mod", lval, rval);
        break;
    case '^':
        print_instruction("xor", lval, rval);
        break;
    case '|':
        print_instruction("or", lval, rval);
        break;
    case '&':
        print_instruction("and", lval, rval);
        break;
    case '<':
        cmp("jl", JL, lval, rval);
        break;
    case '>':
        cmp("jg", JG, lval, rval);
        break;
    case _and:
        print_instruction("land", lval, rval);
        break;
    case _or:
        print_instruction("lor", lval, rval);
        break;
    case _eq:
        cmp("je", EQ, lval, rval);
        break;
    case _neq:
        cmp("jne", NEQ, lval, rval);
        break;
    case _meq:
        cmp("jge", MEQ, lval, rval);
        break;
    case _leq:
        cmp("jle", LEQ, lval, rval);
        break;
    case _lshift:
        print_instruction("lsh", lval, rval);
        break;
    case _rshift:
        print_instruction("rsh", lval, rval);
        break;
    default:
        printf("# No operation found, %d(%c).\n", op, op);
        break;
    }

    //struct reg *ret_val = wrap_number(*lval_value);

    release_register(&rval);
    //release_register(lval);

    return lval;
}

struct reg *cmp(const char *jmp_ins, int jmp_id, struct reg *lvalue, struct reg *rvalue)
{
    char true_label[64];
    char false_label[64];

    snprintf(true_label, 64, "%s_true%u", jmp_ins, lables_cnt[jmp_id]);
    snprintf(false_label, 64, "%s_false%u", jmp_ins, lables_cnt[jmp_id]++);

    print_instruction("cmp", lvalue, rvalue);
    /*
    Example: a < b

        cmp lvalue, rvalue
        mov lvalue, a
        mov rvalue. b
        jl jl_true0
        mov lvalue, 0
        jmp jl_false0
    jl_true0:
        mov lvalue, 1
    jl_false0:
    */
    fprintf(asm_file, "    %s %s\n", jmp_ins, true_label);
    movi(lvalue, 0, COPY_UNWR);
    fprintf(asm_file, "    jmp %s\n", false_label);
    fprintf(asm_file, "%s:\n", true_label);
    movi(lvalue, 1, COPY_UNWR);
    fprintf(asm_file, "%s:\n", false_label);
}

void movi(struct reg *lvalue, int rvalue, int unwr_type)
{
    if (lvalue->is_wrap)
        unwrap_register(lvalue, unwr_type);

    if (lvalue->is_ptr == 1)
        fprintf(asm_file, "    movi [%s], %d\n", lvalue->name, rvalue);
    else
        fprintf(asm_file, "    movi %s, %d\n", lvalue->name, rvalue);
}

void test(struct reg *lvalue, struct reg *rvalue)
{
    if (lvalue->is_wrap)
        unwrap_register(lvalue, COPY_UNWR);
    if (rvalue->is_wrap)
        unwrap_register(rvalue, COPY_UNWR);

    print_instruction("test", lvalue, rvalue);

    //release_register(lvalue);
   // release_register(rvalue);
}

void ret(struct reg *reg)
{
    if (reg->is_wrap)
        unwrap_register(reg, COPY_UNWR);

    if (reg->is_ptr == 1)
        fprintf(asm_file, "    ret [%s]\n", reg->name);
    else
        fprintf(asm_file, "    ret %s\n",   reg->name);
}

void pnt(struct reg *reg)
{
    if (reg->is_wrap)
        unwrap_register(reg, COPY_UNWR);

    if (reg->is_ptr == 1)
        fprintf(asm_file, "    pnt [%s]\n", reg->name);
    else
        fprintf(asm_file, "    pnt %s\n", reg->name);
}