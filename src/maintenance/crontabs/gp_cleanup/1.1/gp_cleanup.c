#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_OPTIONS	4
static char onearg[MAX_OPTIONS][255]={
	"-help",
	"-database",
	"-remove",
	"-since"
};

static char GenPedRoot[255]={""};
static char GenPedDB[255]={""};
static char ListOnly=1;
static time_t Sometime=0;
static long Since=0;

void print_usage(char *progname)
{
  printf("gp_cleanup V1.0 - Copyright(c) 1999,2000 GenPed Consulting\n");
  printf(" -help\t\t this page\n");
  printf(" -database [GenPedDB]\t name of this GenPed database\n");
  printf(" -since [HH]\t cleanup temp file older than [HH] hours\n");
  printf(" -remove\t really remove temporary files (mandatory)\n");
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
	case 2: ListOnly=0;
		break;
	case 3: j++;
		if (j<argc)
		{ 
		  if (sscanf(argv[j],"%ld",&Since)!=1)
		    Since=0;	/* Invalid number of hours */
		  else
		  {
		    if (Since<=0)
		      Since=0; /* Only positive number of hours allowed */
		    else
		    {
		      Since=Sometime-Since*3600; /* Date in seconds */
		      printf("Removing Temporary Files created before %s",
			ctime(&Since));
		    }
	 	  }
		}
		break;
     }
  }
  j++;
 }
 if (Since==0)
 {
	print_usage(argv[0]);
	exit(1);
 }
}

int main(int argc, char **argv)
{
 struct stat buf;
 DIR *TmpDir=NULL;
 struct dirent *DirEnt=NULL;
 char onefile[255]={""};
 char dirname[255]={""};
 int i=1;
 Sometime=time(&Sometime);
 process_options(argc,argv);
 
#if VERBOSE
 printf("Removing Temporary Files for GenPedDB [%s] at %s",
		 GenPedDB,ctime(&Sometime));
#endif
 
 sprintf(dirname,"%s/data/%s/tmp", GenPedRoot, GenPedDB);
 if (stat(dirname,&buf)<0)	/* Can not stat dirname */
	return -1;
 if (!(S_ISDIR(buf.st_mode)))	/* Check if tmp directory is present */
	printf("Error: Can not scan Temporary Spool Directory\n");


 if ((TmpDir=(DIR *)opendir(dirname))==NULL)
	return -2;
 while ((DirEnt=readdir(TmpDir))!=NULL)
 {
   if (DirEnt->d_name[0]!='.')	/* Only plain files */
   {
     sprintf(onefile,"%s/%s",dirname,DirEnt->d_name);
     if (stat(onefile,&buf)<0)
	printf("Error: Can't stat [%s]\n", DirEnt->d_name);
     if (buf.st_mtime< Since)
     {
      if (ListOnly)
	printf("TO REMOVE [%s] ->%s",DirEnt->d_name, ctime(&(buf.st_mtime)));
      else
	remove(onefile);
     }
     else
     {
	printf("TO KEEP [%s] ->%s",DirEnt->d_name, ctime(&(buf.st_mtime)));
     }
   }
 }
 
}
