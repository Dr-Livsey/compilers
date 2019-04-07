#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern int yyparse();
extern FILE *yyin;
extern FILE *yyout;

int main(int argc, char **argv) 
{
    if (argc != 2)
    {
        printf("Please, specify the source file. (*.rs)");
        exit(EXIT_FAILURE);
    }
    else
    {
        yyin = fopen(argv[1], "r");
        yyout = stdout;

        if (yyin == NULL)
        {
            printf("Can't open \"%s\".\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        else
        {
            int retval = yyparse();
            switch (yyparse())
            {
                case 0:
                    printf("\"%s\": done.\n", argv[1]);
                    break;
                case 1:
                    printf("\"%s\": fail.\n", argv[1]); 
                    break;
                default:
                    break;
            }

            return retval;
        }
    }

    return 0;
}