# Location for various files
CC=gcc
BINDIR=.
OBJDIR=.
LIBDIR=.
INCDIR=.
CONFIGLIB = -lgpconfig
CFLAGS= -g
SYBASE  = /usr/local/openclient
LIBS= -L$(LIBDIR) $(CONFIGLIB) -lm -lnsl -lc
INCLUDE_DIR= -I$(INCDIR)

OBJECTS = gpconfig.o

all:	libgpconfig.a example

gpconfig.o: vars.h gpconfig.c gpconfig.h
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -c gpconfig.c

libgpconfig.a: $(OBJECTS)
	@ar -rcs libgpconfig.a $(OBJECTS)

example: libgpconfig.a example.c
	@echo "Creating GenPed Config Example Program..."
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -o example example.c $(LIBS)

clean:
	@echo "Cleaning GenPed Config Library..."
	@rm -f $(OBJECTS) core *.o libgpconfig.a example
