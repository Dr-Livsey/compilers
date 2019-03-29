#include "heap.h"
#include <memory>



void deallocate(void *var_ptr)
{
    delete (variable*)var_ptr;
}

int hlist_match(void *a, void *b)
{
    return (*(variable *)a) == (*(variable *)b) ? 1 : 0;
}

void Heap::allocate(const std::string &_name, int32_t _value)
{
    variable *new_var = new variable(_name, _value);
    list_lpush(heap_list, list_node_new(new_var));
}

variable *Heap::get_variable(const std::string &_name)
{
    variable temp_var(_name, 0);

    list_node_t *node = list_find(heap_list, (variable*)&temp_var);

    return ((variable*)node->val);
}