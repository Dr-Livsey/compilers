#ifndef ASM_PARSE_h
#define ASM_PARSE_H

#include "y.tab.h"

/*get variable copy*/
#define COPY_UNWR 0

/*get variable address*/
#define ADDR_UNWR 1

#define reg_free(x)    reg_mask & (1 << x)
#define reg_lock(x)    reg_mask &= (~(1 << x))
#define reg_release(x) reg_mask |= (1 << x)

char *reg_names[8];
int reg_mask;

/*get first free register*/
char *lock_register();

/*get register wrap from number*/
struct reg *wrap_number(int value);

/*get register wrap from variable*/
struct reg *wrap_variable(struct var *variable);

/*assign name to register*/
void unwrap_register(struct reg *reg, int type);

int *get_reg_value(struct reg *reg);

void print_instruction(const char *ins, struct reg *fst, struct reg *sec);

void free_register(void *reg);

/*find register in list and call free_register()*/
void release_register(struct reg **reg);

int reglist_match(void *a, void *b);

#endif