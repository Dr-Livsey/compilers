CC=cc

#linked list for variables
LLIST_SRC=linked-list/list.c \
			linked-list/list_node.c \
				linked-list/list_iterator.c

SRC =    asm_parse.c  instruction.c op_parse.c
HEADER = asm_parse.h  instruction.h

#yacc sources
YACC_SRC=y.tab.c y.tab.h

#lex sources
LEX_SRC=lex.yy.c

#my compiler name
TARGET=compiler

CODE=code.z

all: build clean

build: lex yacc $(YACC_SRC) $(LEX_SRC) $(ASM_PARSE_SRC) $(LLIST_SRC) $(SRC) $(HEADER)
		@$(CC) $(YACC_SRC) $(LEX_SRC) $(LLIST_SRC) $(SRC) $(HEADER) -o $(TARGET)

lex:	test.l
		@lex $?

yacc: test.y
		@yacc -d -t -v $?

clean:
		@rm $(YACC_SRC) $(LEX_SRC)

run: $(TARGET) $(CODE)
		clear
		@./$(TARGET) $(CODE)

leaks: $(TARGET) $(CODE)
		@valgrind --track-origins=yes --log-file=./valgrind.txt --leak-check=full  ./$(TARGET) $(CODE)