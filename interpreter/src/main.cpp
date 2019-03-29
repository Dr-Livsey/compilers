#include <iostream>
#include <functional>
#include "program.h"


int main(int argc, char *argv[]) 
{
    if (argc != 2)
    {
        std::cout << "Pleasy, specify  *.asm file.\n";
        return 1;
    }

    try
    {
        Program main_program(argv[1]);

        main_program.execute();
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nEx. #" << e.what() << '\n';
    }
    catch (const std::string &msg)
    {
        std::cerr << "\n" <<  msg << '\n';
    }


    
    return 0;
}