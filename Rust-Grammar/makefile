

LEX_SRC=lex.yy.c
YACC_SRC=y.tab.c y.tab.h
EXEC=parser.exe

RUST_SOURCE=rust_test/src/main.rs

all: lex yacc
	gcc $(LEX_SRC) $(YACC_SRC) main.c -o parser.exe
	rm -r $(LEX_SRC) $(YACC_SRC)

lex: parser.l
	flex parser.l

yacc:
	bison -dy parser.y

run:
	@clear
	@./parser.exe $(RUST_SOURCE)

clean:
	rm -r $(LEX_SRC) $(YACC_SRC)

cleanall:
	rm -r $(LEX_SRC) $(YACC_SRC) $(EXEC)