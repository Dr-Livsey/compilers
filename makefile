

LEX_SRC=lex.yy.c
YACC_SRC=y.tab.c y.tab.h
EXEC=parser.exe

RUST_SOURCE=../src/main.rs

all: lex yacc
	gcc $(LEX_SRC) $(YACC_SRC) -o parser.exe

lex: parser.l
	flex parser.l

yacc:
	bison -dy parser.y

run:
	@clear
	@./parser.exe "../src/main.rs"

clean:
	rm -r $(LEX_SRC) $(YACC_SRC)

cleanall:
	rm -r $(LEX_SRC) $(YACC_SRC) $(EXEC)