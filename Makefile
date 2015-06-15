#!/usr/bin/make

# The toplevel directory of the source tree.
SRCDIR = ./src

# The toplevel directory of the tests tree.
TSTDIR = ./tests

# The directory into which object code files should be written.
OBJDIR = ./obj

# The directory into which executable file should be written.
BINDIR = ./bin

# C Compiler and options for use in building executable.
# CC = gcc -g -Wall # <-- Debug.
CC = gcc -O3 -Wall # <-- Release.

APP = donna_vista
TAP = test_$(APP)
SRC = $(SRCDIR)/$(APP).c
OBJ = $(OBJDIR)/$(APP).o

all: $(OBJDIR) $(APP)

$(OBJDIR):
	-mkdir $(OBJDIR)
	-mkdir $(BINDIR)

$(APP):
	$(CC) -o $(OBJDIR)/$(APP).o -c $(SRCDIR)/$(APP).c
	$(CC) -o $(BINDIR)/$(APP) $(OBJ)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/$(APP)
	rm -f $(BINDIR)/$(TAP)
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

test:
	$(TSTDIR)/clar/generate.py $(TSTDIR)
	$(CC) -Wall -I$(TSTDIR) -I$(SRCDIR) -o $(OBJDIR)/test_main.o -c ./tests/main.c
	$(CC) -Wall -I$(TSTDIR) -I$(SRCDIR) -o $(OBJDIR)/test_clar.o -c ./tests/clar.c
	$(CC) -o $(BINDIR)/$(TAP) $(OBJDIR)/test_main.o $(OBJDIR)/test_clar.o
	$(BINDIR)/$(TAP)
