# For gcc
CC= gcc
# For ANSI compilers
#CC= /opt/SUNWspro/bin/cc

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

GPCONFIG_DIR = ../../../libs/gpconfig/current
GPHTML_DIR = ../../../libs/gphtml/current
LIBPED_DIR = ../../../libs/libped/current

RM= /bin/rm -f

INSTDIR = ../../../../cgi-bin

INCDIR	= $(GPCONFIG_DIR) -I$(GPHTML_DIR)
LIBDIR	= $(GPCONFIG_DIR) -lgpconfig -L$(GPHTML_DIR) -lgphtml

OBJECTS = util.o mainpage.o pending.o users.o questions.o

all: gpadmin done

util.o: util.c util.h
	@$(CC) -c util.c

mainpage.o: mainpage.c mainpage.h
	@$(CC) -c mainpage.c -I$(INCDIR)

pending.o: pending.c
	@$(CC) -c pending.c -I$(INCDIR)

users.o: users.c
	@$(CC) -c users.c -I$(INCDIR)

questions.o: questions.c questions.h
	@$(CC) -c questions.c -I$(INCDIR)

gpadmin: gpadmin.c $(OBJECTS)
	@$(CC) -I/opt2/sybase/include -I/usr/include -I$(INCDIR) gpadmin.c util.o mainpage.o pending.o users.o questions.o -L$(LIBDIR) -o gpadmin -lm -lnsl -lgpconfig

install: gpadmin
	@cp gpadmin $(INSTDIR)/gpadmin
	@echo "GenPed Administrator Web Interface Program installed."

clean:
	@rm -f *.o gpadmin 
	@echo "GenPed Administrator Web Interface Program cleaned."

done:
	@echo "GenPed Administrator Web Interface Program created."
