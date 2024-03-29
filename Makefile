CC=g++
SRCDIR=./src
OBJDIR=./obj
CFLAGS=-I$(SRCDIR) -Wall -g -o
HEADERS=lexer.hpp syntax.hpp
SOURCES=lexer.cpp syntax.cpp main.cpp
TARGET=lex
OBJ:=$(SOURCES:.cpp=.o)
OBJ:=$(addprefix $(OBJDIR)/,$(OBJ))

vpath %.cpp $(SRCDIR)
vpath %.hpp $(SRCDIR)

$(TARGET) : $(OBJ) 
	$(CC) $(CFLAGS) $(TARGET) $^

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $@ $<

clean:  
	rm -f $(OBJ) $(TARGET)
