#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_OPTIONS	3
static char onearg[MAX_OPTIONS][255]={
	"-help",
	"-database",
	"-listonly"
};

static char GenPedRoot[255]={""};
static char GenPedDB[255]={""};
static char ListOnly=0;

void print_usage(char *progname)
{
  printf("%s V1.0 - Copyright(c) 1999,2000 GenPed Consulting\n",progname);
  printf(" -help\t\t this page\n");
  printf(" -database\t name of this GenPed database\n");
  printf(" -listonly\t do not perform cleaning, just verbose\n");
}

int get_option(char *strarg)
{
 int i=0;
  for (i=0; i<MAX_OPTIONS; i++)
    if (strcmp(strarg,onearg[i])==0)
	return i;
 return -1;
}

void process_options(int argc, char **argv)
{
int i=0;
int j=1;
 if (getenv("GENPED_ROOT_DIR")!=NULL)
   strcpy(GenPedRoot,getenv("GENPED_ROOT_DIR"));
 else
   strcpy(GenPedRoot,"/home/genped");

 while (j<argc)
 {
   if ((i=(int)get_option(argv[j]))>=0)
   {
     switch(i)
     {
	case 0: print_usage(argv[0]);
		break;
	case 1: j++;
		if (j<argc)
		{
		  if (strcpy(GenPedDB,argv[j])==NULL)
			strcpy(GenPedDB,"");
		}
		break;
	case 2: ListOnly=1;
		break;
     }
  }
  j++;
 }
}

int main(int argc, char **argv)
{
 struct stat buf;
 int i=1;
 time_t Sometime=0;
 Sometime=time(&Sometime);
 process_options(argc,argv);
 
 printf("Removing Temporary Files for GenPedDB [%s] at %s",
		 GenPedDB,ctime(&Sometime));
}
