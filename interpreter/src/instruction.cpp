#include "instruction.h"
#include "program.h"
#include <iostream>

enum
{
    l,
    le,
    g,
    ge,
    e,
    ne,
    z
};
bool compare_values[] = {false, false, false, false, false, false, false};

int64_t *arg::unpack(void)
{
    type dummy_t;
    return unpack(dummy_t);
}

int64_t *arg::unpack(arg::type &_t)
{
    _t = t;

    switch (_t)
    {
        case arg::value:
        case arg::reg:
        case arg::var:
        {
            return ((int64_t *)data);
        }
        case arg::ptr:
        {
            return ((int64_t *)(int64_t)(*(int64_t *)data));
        }
        case arg::label:
        case arg::empty:  
        default:
            throw std::string("# Unexpected type. Error while unpacking arg.");
    }
}

unsigned label::jump(void)
{
    if (can_jump == false)
        throw std::string("# Uninitialized label. Jump address is invalid.");

    return jmp_addr;
}

unsigned JumpInstruction::execute()
{
  //  std::cout << "jump instruction -- " << name << std::endl;

    //(jl|jle|jg|jge|je|jne|jz
    struct label *label = lvalue.to_label();

    if (label == NULL)
        throw std::string("# Error while unpacking label. Line = " + std::to_string(addr));

    if (name == "jl"){
        return (compare_values[l] == true) ? label->jump() : addr + 1;
    }
    else if (name == "jle"){
        return (compare_values[le] == true) ? label->jump() : addr + 1;
    }
    else if (name == "jg"){
        return (compare_values[g] == true) ? label->jump() : addr + 1;
    }
    else if (name == "jge"){
        return (compare_values[ge] == true) ? label->jump() : addr + 1;
    }
    else if (name == "je"){
        return (compare_values[e] == true) ? label->jump() : addr + 1;
    }
    else if (name == "jne"){
        return (compare_values[ne] == true) ? label->jump() : addr + 1;
    }
    else if (name == "jz"){
        return (compare_values[z] == true) ? label->jump() : addr + 1;
    }
    else if (name == "jmp"){
        return label->jump();
    }

    throw std::string("# Jump instruction execute() is unexpected.");
    return 0;
}

unsigned AssignInstruction::execute()
{
  //  std::cout << "assign instruction -- " << name << std::endl;

    //(movi|add|sub|mul|div|mod|xor|or|and|land|lor|lsh|rsh)
    if (name == "movi"){
       (*lvalue.unpack()) = (int64_t)rvalue.unpack();
    }
    else if (name == "mov" || name == "ld"){
        (*lvalue.unpack()) = (*rvalue.unpack());
    }
    else if (name == "lea"){
       (*lvalue.unpack()) = (int64_t)rvalue.unpack();
    }
    else if (name == "add"){
        (*lvalue.unpack()) += (*rvalue.unpack());
    }
    else if (name == "sub"){
        (*lvalue.unpack()) -= (*rvalue.unpack());
    }
    else if (name == "mul"){
        (*lvalue.unpack()) *= (*rvalue.unpack());
    }
    else if (name == "div"){
        (*lvalue.unpack()) /= (*rvalue.unpack());
    }
    else if (name == "mod"){
        (*lvalue.unpack()) %= (*rvalue.unpack());
    }
    else if (name == "xor"){
        (*lvalue.unpack()) ^= (*rvalue.unpack());
    }
    else if (name == "or"){
        (*lvalue.unpack()) |= (*rvalue.unpack());
    }
    else if (name == "and"){
        (*lvalue.unpack()) &= (*rvalue.unpack());
    }
    else if (name == "land"){
        (*lvalue.unpack()) = ((*lvalue.unpack()) && (*rvalue.unpack())) ? 1 : 0;
    }
    else if (name == "land"){
        (*lvalue.unpack()) = ((*lvalue.unpack()) || (*rvalue.unpack())) ? 1 : 0;
    }
    else if (name == "lsh"){
        (*lvalue.unpack()) <<= (*rvalue.unpack());
    }
    else if (name == "rsh"){
        (*lvalue.unpack()) >>= (*rvalue.unpack());
    }

    return addr + 1;
}

unsigned CompareInstrution::execute()
{
   // std::cout << "compare instruction -- " << name << std::endl;

    int left_val = *lvalue.unpack();
    int right_val = *rvalue.unpack();

    std::fill(compare_values, compare_values + sizeof(compare_values)/sizeof(bool), false);


    if (name == "cmp"){
        //(jl|jle|jg|jge|je|jne|jz
 
        if (left_val < right_val)
            compare_values[l] = true;

        if (left_val <= right_val)
            compare_values[le] = true;

        if (left_val > right_val)
            compare_values[g] = true;

        if (left_val >= right_val)
            compare_values[ge] = true;

        if (left_val == right_val)
            compare_values[e] = true;

        if (left_val != right_val)
            compare_values[ne] = true;
    }
    else if (name == "test"){
        if ((left_val & right_val) == 0)
            compare_values[z] = true;
    }

    return addr + 1;
}

unsigned SingleArgInstruction::execute()
{
    //std::cout << "single arg instruction -- " << name << std::endl;

    if (name == "pnt"){
        std::cout << (*lvalue.unpack()) << std::endl;
    }
    else if (name == "ret"){
        throw return_ex((*lvalue.unpack()));
    }
    else if (name == "inc" || name == "dec"){
        (*lvalue.unpack()) += (name == "inc") ? 1 : -1;
    }


    return addr + 1;
}