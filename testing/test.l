%{
	#include <stdio.h>
	#include <malloc.h>
	#include <stdlib.h>
	#include "y.tab.h"
	#include "asm_parse.h"

    //#define DEBUG

	void yyerror(char *);
	int assign_token(char *str);
	int key_word(char *);

	void varlist_free(void *val);
	int varlist_match(void *a, void *b);

	unsigned sym_count = 0;
	unsigned errors = 0;

	FILE *asm_file = NULL;

	list_t *var_list = NULL;
	list_t *reg_list = NULL;
	list_t *labels_stack = NULL;
	list_t *ifstart_stack = NULL;

	unsigned scope_counter = 0;
%}

d  [0-9]
w [a-zA-Z]

%%

{d}+		{ 
				yylval.value = atoi(yytext);	
				sym_count += yyleng; 
				return digit_token; 
			}

(while|if|else|return|print|break)	  { 
				sym_count += yyleng; return key_word(yytext);
			}
(>|<) 		{ sym_count += yyleng; return *yytext; }

(>=|<=)		{ sym_count += yyleng;  return (!strcmp(yytext, ">=")) ? _meq : _leq; }

(!=|==)		{ sym_count += yyleng;  return (!strcmp(yytext, "!=")) ? _neq : _eq; }

(=|\+=|-=|\*=|\/=|^=|\|=|&=|<<=|>>=|%=)   { 	
				sym_count += yyleng; return assign_token(yytext);
			} 	

[!~]		 { sym_count += yyleng; return *yytext; }

(--|\+\+)	 { sym_count += yyleng;  return (!strcmp(yytext, "--")) ? _dec : _inc; }

(&&|\|\|)	 { sym_count += yyleng;  return (!strcmp(yytext, "&&")) ? _and : _or; }
([-+*^/|&%]) { sym_count += yyleng;  return *yytext; }
(<<|>>)		 { sym_count += yyleng;  return (!strcmp(yytext, "<<")) ? _lshift : _rshift; }

_*[a-zA-Z0-9]+	{  
				struct var *variable = (struct var*)malloc(sizeof(struct var));

				char *var_name = (char*)malloc(sizeof(char) * (strlen(yytext) + 1));
				memcpy(var_name, yytext, sizeof(char) * (strlen(yytext) + 1));

				variable->name = var_name;
				variable->is_exists = 0;
				variable->value = 0;
		
				list_node_t *node = list_find(var_list, (void*)variable);

				if (node)
				{
					free(var_name);
					free(variable);
					yylval.var = (struct var*)node->val;
				}
				else yylval.var = variable;

				sym_count += yyleng; 
				return var_token;
			}
[ \t] 			sym_count += yyleng;/* skip whitespace */
[;(){},]	{
				if (!strcmp(yytext, "{"))
					fprintf(asm_file, ".scope%u\n", ++scope_counter);
				else if (!strcmp(yytext, "}"))
					fprintf(asm_file, ".scope%u_end\n", scope_counter--);

				sym_count += yyleng;
				return *yytext;
			}
[\n]		{ 	
				yylineno++; sym_count = 0;
			} 

.			{ 	sym_count++; return unknown_token; }
%%

int assign_token(char *keyword)
{
	if (!strcmp(keyword, "=")) return '=';
	else if (!strcmp(keyword, "+="))  return _asum;
	else if (!strcmp(keyword, "-="))  return _asub;
	else if (!strcmp(keyword, "*="))  return _amul;
	else if (!strcmp(keyword, "/="))  return _adiv;
	else if (!strcmp(keyword, "^="))  return _axor;
	else if (!strcmp(keyword, "|="))  return _aor;
	else if (!strcmp(keyword, "&="))  return _aand;
	else if (!strcmp(keyword, "<<=")) return _alshift;
	else if (!strcmp(keyword, ">>=")) return _arshift;
	else if (!strcmp(keyword, "%="))  return _amod;
}

int key_word(char *keyword)
{
	if (!strcmp(keyword, "while")) return _while;
	else if (!strcmp(keyword, "if")) return _if;
	else if (!strcmp(keyword, "else")) return _else;
	else if (!strcmp(keyword, "return")) return _return;
	else if (!strcmp(keyword, "print")) return _print;
	else if (!strcmp(keyword, "break")) return _break;
}
int yywrap(void) 
{
  	return 1;
} 

int main(int argc, char *argv[]) 
{
#ifdef DEBUG
	yydebug = 1;
#endif

	yyin = fopen(argv[1], "r");
	yyout = stdout;

	if (yyin == NULL)
	{
		printf("Can't open file \"%s\". \n", argv[1]);
		return 1;
	} 

	char *asm_fname = strtok(argv[1], ".");
	char out_name[64];

	if (asm_fname != NULL)
	{
		snprintf(out_name, 64, "%s.asm", asm_fname);
		asm_file = fopen(out_name, "w");
	}
	else
	{
		printf("Unrecognized source file. (Must be *.z)\n");
		return 1;
	}

	
	var_list = list_new();
	var_list->free = &varlist_free;
	var_list->match = &varlist_match;

	reg_list = list_new();
	reg_list->free = &free_register;
	reg_list->match = &reglist_match;

	labels_stack = list_new();
	ifstart_stack = list_new();

	fprintf(asm_file, ".scope0\n");
 	yyparse();
	fprintf(asm_file, ".scope0_end\n");

	fclose(yyin);
	fclose(asm_file);

	list_destroy(var_list);
	list_destroy(reg_list);
	list_destroy(labels_stack);
	list_destroy(ifstart_stack);


	if (!errors)
		printf("\"%s.z\": done.\n", argv[1]);
	else
		printf("\"%s.z\": fail.\n", argv[1]);
 	return 0;
} 

void varlist_free(void *val)
{
   //printf("- var %s free\n", ((struct var*)val)->name);
   free(((struct var*)val)->name);
   free(val);
}

int varlist_match(void *a, void *b)
{
	struct var *a_var = (struct var*)a;
	struct var *b_var = (struct var*)b;

	if (a_var && b_var && !strcmp(a_var->name, b_var->name))	return 1;
	
	return 0;
}