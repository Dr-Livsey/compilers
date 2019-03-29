%{
	#include "y.tab.h"

	unsigned errors = 0;
%}
%%
%%
int yywrap(void) 
{
  	return 1;
} 

int main(int argc, char *argv[]) 
{
	yyin = fopen(argv[1], "r");
	yyout = stdout;

	if (yyin == NULL)
	{
		printf("Can't open file \"%s\". \n", argv[1]);
		return 1;
	} 

	if (!errors)
		printf("\"%s\": done.\n", argv[1]);
	else
		printf("\"%s\": fail.\n", argv[1]);
 	return 0;
} 