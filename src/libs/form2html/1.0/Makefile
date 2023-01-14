# Location for various files
CC=gcc -g
BINDIR=.
OBJDIR=.
LIBDIR=.
INCDIR=.
THISLIB = -lform2html
CFLAGS= -g
LIBS= -L$(LIBDIR) $(THISLIB) -lgd -lm -lnsl
INCLUDE_DIR= -I$(INCDIR)

OBJECTS = form2html.o

all: echoit libform2html.a print_form done

echoit:
	@echo "Making Form2HTML Library "

form2html.o: form2html.c form2html.h
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -c form2html.c

libform2html.a: $(OBJECTS)
	@ar -rs libform2html.a $(OBJECTS)

print_form: libform2html.a print_form.c
	@$(CC) $(INCLUDE_DIR) $(CFLAGS) -o print_form print_form.c $(LIBS)

clean: echoit
	@rm -f $(OBJECTS) core *.o *.a print_form
	@echo "Form2HTML Library cleaned"

done:
	@echo "created."
