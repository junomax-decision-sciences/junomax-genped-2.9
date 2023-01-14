
CFLAGS= -O2

all: gp_cleanup

gp_cleanup: gp_cleanup.c
	@cc $(CFLAGS) -o gp_cleanup gp_cleanup.c

clean:
	@rm -f gp_cleanup *.o core

install:
	@cp -f gp_cleanup /home/genped/bin
	@chmod 2770 /home/genped/bin/gp_cleanup
