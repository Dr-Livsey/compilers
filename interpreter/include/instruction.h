#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "../include/list.h"
#include <iostream>
#include <string>

struct label;

class return_ex : std::exception
{
public:
    return_ex(int _retval) : retval(_retval) {}

    void message(){
        std::cout << "Returning from program. Exit code : " << retval << std::endl;
    }

private:
    int retval;
};

struct arg
{
    enum type {reg, value, ptr, empty, label, var};

    void set_arg(type _t, void *_data)
    {
         t = _t; 
         data  = _data; 
    }

    int64_t *unpack(type &_t);
    int64_t *unpack(void);

    struct label *to_label() { return (t == arg::label) ? ((struct label*)data) : NULL; }

  private:
    type t;
    void *data;
};

struct label
{
    label(std::string _name, unsigned _jmp_addr)
        : can_jump(false), name(_name), jmp_addr(_jmp_addr)
    {}

    unsigned jump();

    bool operator==(const label &other){ return other.name == name; }

    void set_addr(unsigned value) {
        jmp_addr = value;
        can_jump = true;
    }

private:
    bool can_jump;
    std::string name;
    unsigned jmp_addr;
};

class Instruction
{
public:
    Instruction(const std::string &n, arg lv, arg rv, uint32_t _addr)
    {
        name = n;
        lvalue = lv;
        rvalue = rv;
        addr = _addr;
    }
    
    virtual unsigned execute() = 0;

protected:
    std::string name;
    uint32_t addr;
    arg lvalue;
    arg rvalue;
};

class JumpInstruction : public Instruction
{
public:
    JumpInstruction(const std::string &n, arg lv, arg rv, uint32_t _addr)
        : Instruction(n, lv, rv, _addr) {}

    unsigned execute() override;
};


class AssignInstruction : public Instruction
{
public:
    AssignInstruction(const std::string &n, arg lv, arg rv, uint32_t _addr)
        : Instruction(n, lv, rv, _addr) {}

    unsigned execute() override;
};

class CompareInstrution : public Instruction
{
public:
    CompareInstrution(const std::string &n, arg lv, arg rv, uint32_t _addr)
        : Instruction(n, lv, rv, _addr) {}

    unsigned execute() override;
};

class SingleArgInstruction : public Instruction
{
  public:
    SingleArgInstruction(const std::string &n, arg lv, arg rv, uint32_t _addr)
        : Instruction(n, lv, rv, _addr) {}

    unsigned execute() override;
};

#endif