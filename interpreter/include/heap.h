#include "../include/list.h"
#include <string>

struct variable
{
    variable(const std::string &n, int32_t _value)
        : name(n), value(_value)
    {}

    bool operator==(const variable &other) {    return other.name == this->name;    }

    int32_t* get_value() {   return &value; }

private:
    std::string name;
    int32_t value;
};

void deallocate(void *var_ptr);
int hlist_match(void *a, void *b);

class Heap
{
public:
    Heap()  
    {  
        heap_list = list_new();     
        heap_list->match = &hlist_match;
        heap_list->free = &deallocate;
    }
    ~Heap() 
    {  
        list_destroy(heap_list);    
    }

    void allocate(const std::string &_name, int32_t _value);
    variable *get_variable(const std::string &_name);
    
private:
    list_t *heap_list;
};