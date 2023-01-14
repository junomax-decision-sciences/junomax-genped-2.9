#  Location for various files
CC=gcc
BINDIR=.
OBJDIR=.
LIBDIR=.
INCDIR=.
LEXCONFIG=../../gpconfig/current

CFLAGS= -g
LIBS= -L$(LIBDIR) $(CONFIGLIB) -lm -lnsl -lc
INCLUDE_DIR= -I$(INCDIR) -I$(LEXCONFIG)

OBJECTS = libgphtml.a 

all:  echohtml  $(OBJECTS) done

echohtml:
	@echo -n "GenPed Html Library "

gphtml.o: gphtml.c
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -c gphtml.c

libgphtml.a: gphtml.o
	@ar -rs libgphtml.a gphtml.o

clean: echohtml
	@rm -f $(OBJECTS) core *.o
	@echo "cleaned."

done:
	@echo "created."
