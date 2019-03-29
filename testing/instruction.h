#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "y.tab.h"
#include "asm_parse.h"

struct reg *execute(int op, struct reg *lval, struct reg *rval);
struct reg *cmp(const char *jmp_ins, int jmp_id, struct reg *lvalue, struct reg *rvalue);

void movi(struct reg *lvalue, int rvalue, int unwr_type);
void test(struct reg *lvalue, struct reg *rvalue);

void ret(struct reg *reg);
void pnt(struct reg *reg);
#endif