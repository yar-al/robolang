CC = g++
CFLAGS = -g -o
LEX = flex
LFLAGS =
YACC = bison
YFLAGS = -d -y -t #-L c++
BIN = test
YFILE = bis.y #biswast.y 
LFILE = lex.l
CFILE = unittest.cpp logic/logic.cpp logic/logic.h logic/types.h ast/ast.h ast/ast.cpp
YOUT = y.tab.c y.tab.h
LOUT = lex.yy.c

.PHONY : all

all : $(BIN)
$(BIN) : $(LOUT) $(YOUT) $(CFILE)
	$(CC) $^ $(CFLAGS) $@
$(LOUT) : $(LFILE)
	$(LEX) $(LFLAGS) $^
$(YOUT) : $(YFILE)
	$(YACC) $(YFLAGS) $^