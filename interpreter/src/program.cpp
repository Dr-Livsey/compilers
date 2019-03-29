#include <iostream>

#include "program.h"
#include "heap.h"
#include "parser.h"
#include "instruction.h"

Program::Program(char *fname)
{
    command_list = list_new();
    label_list = list_new();

    label_list->free = &list_free;
    command_list->free = &list_free;

    label_list->match = &match_label;

    heap = new Heap();

    Parser parser(command_list, label_list, heap);

    parser.parse_asm(fname);
}

Program::~Program()
{
    list_destroy(command_list);
    list_destroy(label_list);

    delete heap;
}

void list_free(void *list_node_val)
{
    delete (struct label*)list_node_val;
}

int match_label(void *la, void *lb)
{
    label *_la = (label*)la;
    label *_lb = (label *)lb;

    return (*_la) == (*_lb) ? 1 : 0;
}

void Program::execute()
{
    list_node_t *cur_node;

    unsigned pc = 0, prev_pc = 0;
    
    while (pc < command_list->len)
    {
        cur_node = (pc == prev_pc + 1) ? cur_node->next : list_at(command_list, pc);

        if (cur_node == NULL)
            throw std::string("# Instruction ptr is NULL");

        Instruction *current_ins = (Instruction *)(cur_node->val);

        prev_pc = pc;
        pc = current_ins->execute();
    }
}