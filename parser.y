%{
	int yylex(void);
	void yyerror(char *);
%}
%%
lines:;
%%
void yyerror(char *s) 
{
 	// fprintf(stderr, "# %s. line = %d, pos = %u\n", s, yylineno, sym_count);
	// errors++;
}