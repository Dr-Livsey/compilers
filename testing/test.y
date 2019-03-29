%{
	#include <stdio.h>
	#include <string.h>
	#include "asm_parse.h"
	#include "instruction.h"

	#define ERROR_DROP(s) yyerror(s);
	int yylex(void);

	extern int 		yylineno; 
	extern unsigned sym_count;
	extern unsigned errors;
	extern list_t *reg_list;
	extern FILE *asm_file;

	extern list_t *labels_stack;
	extern list_t *ifstart_stack;
	extern FILE *yyin;

	enum {IF_START, IF_BODY, WHILE_START};
	unsigned cond_labels[] = {0, 0, 0};
%}

%code requires
{
	#include "linked-list/list.h"

	struct var
	{
		int value;
		char *name;
		int is_exists;
	};

	struct reg
	{
		char *name;
		void *value;
		int is_ptr : 2;
		int is_wrap : 1;
	};

	extern struct reg *operation(int op, struct reg *fst, struct reg *sec);
	extern struct reg *assignment(struct reg *lvalue, int as_op, struct reg *statement);
	extern struct reg *unar_operation(int op, struct reg *fst);
	extern struct reg *increment(struct reg *reg);
	extern struct reg *decrement(struct reg *reg);
	void yyerror(char *);
}

//%expect 128

%start lines
%union 
{
	int value;
	struct var *var;
	struct reg *reg;
}


%token  _meq _leq 
%token  _neq _eq
%token  _or _and
%token  _inc _dec
%token  _lshift _rshift

%token <value> digit_token 
%token <var>   var_token

%token _if _else _while _return _print _break
%token eof_token 0
%token unknown_token "unknown symbol"

%type <reg> statement if_condition 
%type <var> single_var

%right '=' _asum _asub _amul _adiv _amod _alshift _arshift _aand _axor _aor
%left _or
%left _and
%left '|'
%left '^'
%left '&' 
%left _neq _eq
%left '>' '<' _meq _leq 
%left _lshift _rshift
%left '+' '-'
%left '*' '/' '%'
%right '!' '~'
%nonassoc _uminus
%right _prefix
%left  _postfix

%%
lines: 
		lines line
		|
		;

line: 
		statement ';'	{ release_register(&$1); }
		| var_token ';' 
		{ 
			if (!$1->is_exists)
			{ 
				struct reg *ret = assignment(wrap_variable($1), '=', wrap_number(0));
				release_register(&ret);
			}
		}
		| '{' lines '}'
		| if_start cond_operator 
		{ 
			list_node_t *node = list_rpop(ifstart_stack);
			fprintf(asm_file, "if_end%u:\n", (unsigned)(long)node->val); 

			free(node);
		}
		| while
		| return
		| _break ';' 	//{ fprintf(asm_file, "    jmp while_%u_exit\n", cond_labels[WHILE_BODY]); }
		| print
		| error ';'  	{ ERROR_DROP("syntax error") }
		;


while_condition:
		statement
		{
			test($1, $1);
			fprintf(asm_file, "    jz while_%u_exit\n", cond_labels[WHILE_START]);
			release_register(&$1);

			list_rpush(labels_stack, list_node_new((void*)(long)cond_labels[WHILE_START]));
			cond_labels[WHILE_START]++;
		}
		;

while_body:
		body
		{ 
			list_node_t *node = list_rpop(labels_stack);

			fprintf(asm_file, "    jmp while%u\n", (unsigned)(long)node->val);
			fprintf(asm_file, "while_%u_exit:\n",  (unsigned)(long)node->val); 

			free(node);
		}
		;

while_start:
		_while
		{
			fprintf(asm_file, "while%u:\n", cond_labels[WHILE_START]);
		}
		;

while:
		while_start '(' while_condition ')' while_body
		| while_start '(' error ')' while_body {ERROR_DROP("unrecognized condition")}
		;

if_body:
		body			
		{ 
			list_node_t *if_body = list_rpop(labels_stack);
			list_node_t *if_start = list_rpop(ifstart_stack);

			fprintf(asm_file, "    jmp if_end%u\n", (unsigned)(long)if_start->val);
			fprintf(asm_file, "if_body_exit%u:\n",  (unsigned)(long)if_body->val); 

			free(if_body);

			list_rpush(ifstart_stack, if_start);
		}
		;

body:
		statement ';'	{ release_register(&$1); }
		| '{' lines '}'					
		| ';'
		;

if_condition:
		statement 						
		{
			test($1, $1);
			fprintf(asm_file, "    jz if_body_exit%u\n", cond_labels[IF_BODY]);

			list_rpush(labels_stack, list_node_new((void*)(long)cond_labels[IF_BODY]));
			cond_labels[IF_BODY]++;

			release_register(&$1);
		}
		;

if_start:
		_if {list_rpush(ifstart_stack, list_node_new((void*)(long)cond_labels[IF_START]++));}
		;

cond_operator:
 		'(' if_condition ')' if_body 				
		| '(' if_condition ')' if_body _else _if cond_operator		
		| '(' if_condition ')' if_body _else body	
		| '(' error ')' if_body
 		;

statement:
		'(' statement ')'				{ $$ = $2; }

		| _inc single_var %prec _prefix  { $$ = increment(wrap_variable($2)); }
		| _dec single_var %prec _prefix  { $$ = decrement(wrap_variable($2)); }
		| single_var _inc %prec _postfix { $$ = increment(wrap_variable($1)); }
		| single_var _dec %prec _postfix { $$ = decrement(wrap_variable($1)); }

		| '-' statement	%prec _uminus	{$$ = unar_operation('-', $2); }

		| '!' statement					{$$ = unar_operation('!', $2); }
		| '~' statement					{$$ = unar_operation('~', $2); }
		
		| statement '*' statement 		{$$ = operation('*', $1, $3); }
		| statement '%' statement		{$$ = operation('%', $1, $3); }
		| statement '/' statement		{$$ = operation('/', $1, $3); }

		| statement '-' statement		{$$ = operation('-', $1, $3); }
	    | statement '+' statement		{$$ = operation('+', $1, $3); }

		| statement _lshift statement	{$$ = operation(_lshift, $1, $3); }
		| statement _rshift statement	{$$ = operation(_rshift, $1, $3); }

		| statement '>' statement 		{$$ = operation('>', $1, $3); }
		| statement '<' statement 		{$$ = operation('<', $1, $3); }
		| statement _meq statement 		{$$ = operation(_meq, $1, $3); }
		| statement _leq statement 		{$$ = operation(_leq, $1, $3); }

		| statement _eq statement		{$$ = operation(_eq, $1, $3); }
		| statement _neq statement		{$$ = operation(_neq, $1, $3); }

		| statement '&' statement		{$$ = operation('&', $1, $3); }
		| statement '^' statement		{$$ = operation('^', $1, $3); }
		| statement '|' statement		{$$ = operation('|', $1, $3); }

	    | statement _and statement		{$$ = operation(_and, $1, $3); }
		| statement _or statement		{$$ = operation(_or, $1, $3); }

		| var_token '=' statement		{$$ = assignment(wrap_variable($1), '=', $3);   }
		| var_token _asum statement 	{$$ = assignment(wrap_variable($1), _asum, $3); }
		| var_token _asub statement 	{$$ = assignment(wrap_variable($1), _asub, $3); }
		| var_token _amul statement 	{$$ = assignment(wrap_variable($1), _amul, $3); }
		| var_token _adiv statement 	{$$ = assignment(wrap_variable($1), _adiv, $3); }
		| var_token _amod statement 	{$$ = assignment(wrap_variable($1), _amod, $3); }
		| var_token _alshift statement 	{$$ = assignment(wrap_variable($1), _alshift, $3); }
		| var_token _arshift statement 	{$$ = assignment(wrap_variable($1), _arshift, $3); }
		| var_token _aand statement 	{$$ = assignment(wrap_variable($1), _aand, $3); }
		| var_token _axor statement 	{$$ = assignment(wrap_variable($1), _axor, $3); }
		| var_token _aor statement 		{$$ = assignment(wrap_variable($1), _aor, $3); }

		| digit_token 					{ $$ = wrap_number($1); }
		| single_var					{ $$ = wrap_variable($1); }	
		;
	

single_var:
		var_token 
		{
			if ($1->is_exists == 0) 
			{
				char err_str[64];
				snprintf(err_str, 64, "variable \"%s\" is undeclared. execution aborted", $1->name);

				yyerror(err_str);
				free($1->name);
				free($1);
        		YYABORT;
			}
			else $$ = $1;
		}
		;

return:
		_return statement ';' 			{ ret($2); release_register(&$2);}
		| _return ';'					{ fprintf(asm_file, "ret\n"); }
		| _return error ';'				{ ERROR_DROP("syntax error") }
		;
print:
		_print statement ';'			{ pnt($2); release_register(&$2);}
		| _print error ';'				{ ERROR_DROP("syntax error") }
		;

%%
void yyerror(char *s) 
{
 	fprintf(stderr, "# %s. line = %d, pos = %u\n", s, yylineno, sym_count);
	errors++;
}