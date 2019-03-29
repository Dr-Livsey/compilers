#ifndef PARSER_H
#define PARSRER_H

#include "../include/list.h"
#include <regex>

class Instruction;
struct arg;

class Heap;

class Parser
{
public:
    Parser(list_t *_cmd_list, list_t *_label_list, Heap *h) 
        : command_list(_cmd_list), label_list(_label_list), heap(h) {}

    void parse_asm(char *);

    ~Parser() = default;

private:
    Parser(){}

    Instruction *parse_instruction(int pat_id, const std::smatch &matches, uint32_t line_num);
    void *get_regptr(const std::string &str);

    list_t *command_list;
    list_t *label_list;
    Heap *heap;
};

#endif //PARSER_H