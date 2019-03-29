#pragma once
#include "../include/list.h"

#ifndef SHIT
#define SHIT

#endif

class Heap;

void list_free(void *list_node_val);
int match_label(void *la, void *lb);

class Program
{
public:
    Program(char *);
    ~Program();

    void execute();

private:
    Program(){}

    list_t *command_list;
    list_t *label_list;
    Heap *heap;
};
