# Location for various files
CC=gcc -g
BINDIR=.
OBJDIR=.
LIBDIR=.
INCDIR=.
PEDLIB = -lped
CFLAGS= -g
LIBS= -L$(LIBDIR) $(PEDLIB) -lgd -lm -lnsl -lc
INCLUDE_DIR= -I$(INCDIR)

OBJECTS = ped.o ped2gif.o

all: echoit libped.a example done

echoit:
	@echo -n "GenPed Pedigree Library "

ped.o: ped.c ped.h
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -c ped.c

ped2gif.o: ped2gif.c ped.h
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -c ped2gif.c

libped.a: $(OBJECTS)
	@ar -rs libped.a $(OBJECTS)

example: libped.a example.c
	@$(CC) $(INCLUDE_DIR) -I../lexhtml $(CFLAGS) -o example example.c $(LIBS)

clean: echoit
	@rm -f $(OBJECTS) core *.o example libped.a
	@echo "cleaned"

done:
	@echo "created."

run: example
	./example fam1.ped

run2: example
	./example fam2.ped

run3: example
	./example fam3.ped
