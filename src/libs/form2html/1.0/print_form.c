#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main(int argc, char **argv)
{
  if (argc!=2)
  {
	printf("Usage: %s form.conf\n",argv[0]);
	exit(1);
  }
  form2html(argv[1]);
}

