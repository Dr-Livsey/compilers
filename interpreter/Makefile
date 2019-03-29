CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= interpreter

CODE		:= code.z
ASM			:= $(CODE:.z=.asm)
COMPILER	:= ../testing/compiler

LLIST := linked-list


all: $(BIN)/$(EXECUTABLE)

run: $(ASM) $(BIN)/$(EXECUTABLE)
	@echo "\n-- Running --"
	@./$(BIN)/$(EXECUTABLE) $(ASM) 2>run-time.log

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp $(LLIST)/*.cpp
	clear
	$(CXX) $(CXX_FLAGS) $^ -o $@ -I$(INCLUDE) -L$(LIB) $(LIBRARIES) > build.log 2>&1 \
	|| grep '\(warning\|error\)' build.log >| build_errors.log
	clear

gen_asm: $(CODE) $(COMPILER)
	clear
	@echo "\n-- Compiling --"
	@./$(COMPILER) $(CODE)
	

clean: 
	@-rm $(BIN)/* || true
