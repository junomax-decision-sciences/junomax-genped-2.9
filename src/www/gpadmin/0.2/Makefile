# For gcc
CC= gcc
# For ANSI compilers
#CC= /opt/SUNWspro/bin/cc

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

GPCONFIG_DIR =	../../../libs/gpconfig/current
GPHTML_DIR = ../../../libs/gphtml/current
LIBPED_DIR = ../../../libs/libped/current
FORM2HTML_DIR = ../../../libs/form2html/current

RM= /bin/rm -f

INSTDIR = ../../../../admin-bin

INCDIR	= $(GPCONFIG_DIR) -I$(GPHTML_DIR) -I$(LIBPED_DIR) -I$(FORM2HTML_DIR)
LIBDIR	= $(GPCONFIG_DIR) -lgpconfig -L$(GPHTML_DIR) -lgphtml -L$(LIBPED_DIR) -lped -L$(FORM2HTML_DIR) -lform2html -lgd -lm

OBJECTS = util.o mainpage.o

all: gpadmin done

util.o: util.c util.h
	@$(CC) -c util.c

mainpage.o: mainpage.c mainpage.h
	@$(CC) -c mainpage.c -I$(INCDIR)

gpadmin: gpadmin.c $(OBJECTS)
	@$(CC) -I/opt2/sybase/include -I/usr/include -I$(INCDIR) gpadmin.c $(OBJECTS) -L$(LIBDIR) -o gpadmin -lm -lnsl -lgpconfig

install: gpadmin
	@cp gpadmin $(INSTDIR)/gpadmin
	@chmod 2751 $(INSTDIR)/gpadmin
	@echo "Pedigree Collector Web Interface Program installed."

clean:
	@rm -f *.o gpadmin 
	@echo "Pedigree Collector Web Interface Program cleaned."

done:
	@echo "Pedigree Collector Web Interface Program created."
