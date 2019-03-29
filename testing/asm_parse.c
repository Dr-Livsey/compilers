#include <stdio.h>
#include "asm_parse.h"
#include "y.tab.h"

#include "string.h"
#include "malloc.h"

extern FILE *asm_file;
extern list_t *reg_list;
    /*
    There are 8 32-bit registers: eax, ebx, ecx, edx, esi, edi, ebp, esp.
    There are 8 16-bit registers: ax, bx, cx, dx, si, di, bp, sp.
    There are 8 8-bit registers: ah, al, bh, bl, ch, cl, dh, dl.
*/

/*1 - register free, 0 - register busy*/
int reg_mask = 0xff;
char *reg_names[] = { "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp" };

char *lock_register()
{
    for (int reg_index = 0; reg_index < (sizeof(reg_names) / sizeof(char *)); reg_index++)
    {
        if (reg_free(reg_index))
        {
            reg_lock(reg_index);
            return reg_names[reg_index];
        }
    }
    return NULL;
}

void print_instruction(const char *ins, struct reg *fst, struct reg *sec)
{
    fprintf(asm_file, "    %s ", ins);

    if (fst->is_ptr == 1)
        fprintf(asm_file, "[%s], ", fst->name);
    else
        fprintf(asm_file, "%s, ", fst->name);

    if (sec->is_ptr == 1)
        fprintf(asm_file, "[%s]\n", sec->name);
    else
        fprintf(asm_file, "%s\n", sec->name);
}

/*get register wrap from number*/
struct reg *wrap_number(int value)
{
    int *int_ptr = (int*)malloc(sizeof(int));
    struct reg *reg = (struct reg *)malloc(sizeof(struct reg));

    memcpy(int_ptr, &value, sizeof(int));

    reg->value = (void*)int_ptr;
    reg->name = NULL;
    reg->is_ptr = 0;
    reg->is_wrap = 1;

    list_rpush(reg_list, list_node_new(reg));

    return reg;
}

/*get register wrap from variable*/
struct reg *wrap_variable(struct var *variable)
{
    struct reg *reg = (struct reg *)malloc(sizeof(struct reg));
    reg->value = (void*)variable;
    reg->name = NULL;

    /*contains pointer to variable*/
    reg->is_ptr = 1;
    reg->is_wrap = 1;

    list_rpush(reg_list, list_node_new(reg));

    return reg;
}

void unwrap_register(struct reg *reg, int type)
{
    char *name = lock_register();

    if (name == NULL) return;

    reg->name = name;
    reg->is_wrap = 0;
    
    if (reg->is_ptr)
    {
        if (type == ADDR_UNWR)
            fprintf(asm_file, "    lea %s, %s\n", name, ((struct var*)reg->value)->name);
        else if (type == COPY_UNWR)
        {
            fprintf(asm_file, "    ld %s, %s\n", name, ((struct var *)reg->value)->name);      
            reg->is_ptr = 3;
        }       
    }
    else
        fprintf(asm_file, "    movi %s, %d\n", name, *((int *)reg->value));
}

int *get_reg_value(struct reg *reg)
{
    return (reg->is_ptr ? &((struct var*)reg->value)->value : (int*)reg->value);
}

void free_register(void *reg)
{
    if (reg == NULL) return;

    struct reg *reg_ptr = (struct reg*)reg;

    if (!reg_ptr->is_wrap)
    {
        for (int reg_index = 0; reg_index < (sizeof(reg_names) / sizeof(char *)); reg_index++)
        {
            if (strcmp(reg_ptr->name, reg_names[reg_index]) == 0)
            {
                reg_release(reg_index);
            }
        }

    }

    //printf("- reg %s free\n", reg_ptr->name);

    if (!reg_ptr->is_ptr)
        free(reg_ptr->value);

    free(reg);
    reg = NULL;
}

void release_register(struct reg** reg)
{
    if (*reg == NULL)
        return;

    list_node_t *node = list_find(reg_list, *reg);
    list_remove(reg_list, node);

    (*reg) = NULL;
}

int reglist_match(void *a, void *b)
{
    struct reg *a_reg = (struct reg *)a;
    struct reg *b_reg = (struct reg *)b;

    return a_reg == b_reg;
}