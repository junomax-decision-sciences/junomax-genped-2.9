#include <stdio.h>
#include <string.h>

#include "ped.h"


main(int argc, char **argv)
{
PEDIGREE *ped=NULL;
COUPLE *couple=NULL;
INDIV *ind=NULL;
int i=0;
int j=0;


  if (argc!=3)
  {
	printf("Usage: %s pedigreefile giffile.\n",argv[0]);
	exit(1);
  }

  if ((ped=load_pedigree(argv[1],0))==NULL)

  {
	printf("Exiting...\n\n");
        free_pedigree(ped);
	exit(1);
  }
  printf("There are %d individuals in this pedigree.\n",ped->nbindivs);
  for (i=0;i<ped->nbindivs; i++)
  { 
	ind = ped->indivs[i];
      if (ind->generation>=0)
      {
	printf("Generation %d:\t%d %d %d %d %d %d %d %d %d (%e) (%.5f, %.5f)",
		ind->generation,
		ind->id+1, ind->sex, ind->mother+1, ind->father+1, 
		ind->spouse+1, ind->kid+1, ind->sib+1, ind->proband,
		ind->pheno, ind->ratio,
		(double)(ind->x),
		(double)(ind->y));
        if (ind->kid>=0)
		printf("*");
	printf("\n");
      }
  }
  gif2pedigree(ped, argv[2],0);
  exit(0);
}

