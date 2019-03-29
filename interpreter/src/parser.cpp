#include <fstream>
#include <iostream>
#include <string>
#include <regex>

#include "parser.h"
#include "heap.h"
#include "instruction.h"

int64_t esp;
int64_t ebp;
int64_t eax;
int64_t ebx;
int64_t ecx;
int64_t edi;
int64_t edx;
int64_t esi;

void chomp(std::string &s)
{
    std::size_t found = s.find_last_not_of(" \t\f\v\n\r");
    if (found != std::string::npos)
        s.erase(found + 1);

    found = s.find_first_not_of(" \t\f\v\n\r");
    if (found != std::string::npos)
        s.erase(0, found);
}

void Parser::parse_asm(char *asm_f)
{
    std::ifstream asm_file(asm_f);

    if (!asm_file.good())
        throw std::string("Can't open *.asm file.");
    else
    {
        std::cout << "\"" << asm_f << "\"\n\n------STDOUT------\n"; 
    }

    std::string reg_pat = "(\\[?)(eax|ebx|ecx|edx|esi|edi|ebp|esp)(]?)";

    std::string cmd_pat[] =
        {
    /*0*/   "^(movi) +" + reg_pat + ", *(-?\\d{1,})$",

    /*1*/   "^(mov|add|sub|mul|div|mod|xor|or|and|land|lor|lsh|rsh|cmp|test) +" + reg_pat + ", *" + reg_pat + "$",

    /*2*/   "^(pnt|dec|inc) +" + reg_pat + "$",

    /*3*/   "^(lea|ld) +" + reg_pat + ", *(.+)$",

    /*4*/    "^(str) +(.+)$",
    /*5*/    "^(ret) *(.*)$",
    /*6*/    "^(jl|jle|jg|jge|je|jne|jz|jmp) +(.+)$",

            /*scope*/
    /*7*/    "^.scope\\d{1,}$",
    /*8*/    "^.scope\\d{1,}_end$",

            /*while*/
    /*9*/    "^(while\\d{1,}):$",
    /*10*/   "^(while_\\d{1,}_exit):$",
            /*if*/
    /*11*/   "^(if_body_exit\\d{1,}):?$",
    /*12*/   "^(if_end\\d{1,}):?$",
    /*13*/  "^((?:jl|jle|jg|jge|je|jne|jz)_(?:true|false)\\d{1,}):?"};

    std::ofstream log("parse_log.txt");

    for (uint32_t line_idx = 0; asm_file.good();)
    {
        std::string line;
        std::getline(asm_file, line);

        chomp(line);

        if (line == "") continue;

        log << "line " << line_idx + 1 << ". \"" << line << "\" -- ";

        std::smatch matches;
        bool retval;
        uint32_t pat_id = 0;

        for (pat_id = 0; pat_id < sizeof(cmd_pat) / sizeof(std::string); pat_id++)
        {
            if (true == (retval = std::regex_match(line, matches, std::regex(cmd_pat[pat_id]))))
            {
                break;
            }
        }

        if (retval == false)
        {
            log << "\t-\n";
            throw std::string("# Unknown instruction. Line=" + std::to_string(line_idx));
        }
        else 
        {
            log << "\t+\tpattern = " << cmd_pat[pat_id] << std::endl;

            //labels
            if (pat_id >= 9 && pat_id <= 13)
            {
                label *find_this = new label(matches[1].str(), 0);

                list_node_t *node = list_find(label_list, (void*)find_this);

                if (node == NULL)
                {
                    std::cerr << "# labels missmatch. find label : \"" << matches[1].str() << "\". Line = " << line_idx << "\n";

                    find_this->set_addr(line_idx);
                    list_lpush(label_list, list_node_new(find_this));
                }
                else
                {
                    std::cerr << "# labels MATCH. find label : \"" << matches[1].str() << "\". Line = " << line_idx << "\n";
                    ((label *)node->val)->set_addr(line_idx);
                    delete find_this;
                }
            }
            //str
            else if (pat_id == 4)
            {
                heap->allocate(matches[2].str(), 0);
            }
            //If not scope labels.
            else if (pat_id != 7 && pat_id != 8)
            {
                Instruction *new_ins = parse_instruction(pat_id, matches, line_idx);

                if (new_ins == nullptr)
                        throw std::string("# Instruction is NULL. Line = " + std::to_string(line_idx));

                list_rpush(command_list, list_node_new(new_ins));

                line_idx++;
            }
        }
    }
    
    log.close();
}

Instruction *Parser::parse_instruction(int pat_id, const std::smatch &matches, uint32_t line_num)
{
    std::string name;
    arg lvalue;
    arg rvalue;

    if (pat_id >= 0 && pat_id <= 6)
        name = matches[1].str();

    if (pat_id == 0) /*movi*/
    {
        lvalue.set_arg(matches[2].str() == "" ? arg::reg : arg::ptr, get_regptr(matches[3].str()));
        rvalue.set_arg(arg::value, (void *)(int64_t)(atoi(matches[5].str().c_str())));
    }
    else if (pat_id == 1 || pat_id == 2) /*(mov|...) || (pnt|inc|dec)*/
    {
        lvalue.set_arg(matches[2].str() == "" ? arg::reg : arg::ptr, get_regptr(matches[3].str()));

        if (pat_id == 1)
        {
            rvalue.set_arg(matches[5].str() == "" ? arg::reg : arg::ptr, get_regptr(matches[6].str()));
        }
        else 
        {
            rvalue.set_arg(arg::empty, NULL);
        }
    }
    else if (pat_id == 3) //(lea|ld)
    {
        variable *var = heap->get_variable(matches[5].str());

        if (var == NULL)
            throw std::string("# Variable is Null. Line = " + std::to_string(line_num));

        lvalue.set_arg(arg::reg, get_regptr(matches[3].str()));
        rvalue.set_arg(arg::var, (void *)var->get_value());
    }
    else if (pat_id == 5) /*ret*/
    {
       if (matches[2].str() != "")
       {
           lvalue.set_arg(arg::reg, get_regptr(matches[2].str()));
       }

       rvalue.set_arg(arg::empty, NULL);
    }
    else if (pat_id == 6) /*(jmp|...)*/
    {
        label *new_label = new label(matches[2].str(), 0);

        list_node_t *node = list_find(label_list, (void*)new_label);

        if (node == NULL)
        {
            std::cerr << "# Unrecognized label : \"" << matches[2].str() << "\"\n";
            list_lpush(label_list, list_node_new(new_label));
        }
        else
        {
            delete new_label;
            new_label = ((label*)node->val);
        }

        lvalue.set_arg(arg::label, (void *)new_label);
        rvalue.set_arg(arg::empty, NULL);
    }

    switch (pat_id)
    {
        case 0: /*movi*/
        case 3: /*(lea|ld)*/
            return new AssignInstruction(name, lvalue, rvalue, line_num);

        case 1: /*(mov|...)*/
            if (name == "cmp" || name == "test")
            {
                return new CompareInstrution(name, lvalue, rvalue, line_num);
            }
            else
                return new AssignInstruction(name, lvalue, rvalue, line_num);

        case 2: //pnt
        case 5: //ret
            return new SingleArgInstruction(name, lvalue, rvalue, line_num);

        case 6: /*(jmp|...)*/
            return new JumpInstruction(name, lvalue, rvalue, line_num);

        default:
            return nullptr;
    }
}

void *Parser::get_regptr(const std::string &str)
{
    if (str == "eax") 
        return (void*)&eax;
    else if (str == "ebx")
        return (void *)&ebx;
    else if (str == "ecx")
        return (void *)&ecx;
    else if (str == "edx")
        return (void *)&edx;
    else if (str == "esi")
        return (void *)&esi;
    else if (str == "edi")
        return (void *)&edi;
    else if (str == "ebp")
        return (void *)&ebp;
    else if (str == "esp")
        return (void *)&esp;

    return NULL;
}