#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "ped.h"

void free_pedigree(PEDIGREE *p)
{
 int i=0;

  if (p==NULL) return;
  for (i=0;i<p->nbindivs;i++)
	if (p->indivs[i]!=NULL) free(p->indivs[i]);
  free(p);
  return;
}

INDIV *create_indiv()
{
INDIV *ind=NULL;
  if ((ind=(INDIV *)malloc(sizeof(INDIV)))==NULL)
	return NULL;
  ind->id = 0;
  ind->sex= 0;
  ind->father= 0; ind->mother= 0;
  ind->spouse= 0; ind->kid= 0;
  ind->sib=0; ind->proband= 0;
  ind->pheno= 0; ind->generation= -1;
  ind->ratio=(double)0.0;
  ind->x=0.0; ind->y=0.0;
  ind->w=0.0;
  ind->h=0.0;
  ind->xpic=0;
  ind->ypic=0;
  return (INDIV *)ind;
}

INDIV_BOX *create_indivbox()
{
INDIV_BOX *ind=NULL;
  if ((ind=(INDIV_BOX *)malloc(sizeof(INDIV_BOX)))==NULL)
        return NULL;
  ind->indiv_id = 0;
  ind->ratio= 0.0;
  ind->xpos= 0; ind->ypos= 0;
  ind->w= 0; ind->y= -1;
  return (INDIV_BOX *)ind;
}

/* Create an empty couple structure pointer */
COUPLE *create_couple()
{
 COUPLE *c=NULL;
  if ((c=(COUPLE *)malloc(sizeof(COUPLE)))==NULL)
	return NULL;
  c->id=0;
  c->father_id=0;
  c->mother_id=0;
  c->nb_sdaughters=0;
  c->nb_cdaughters;
  c->nb_csons=0;
  c->nb_ssons=0;
  return (COUPLE *)c;
}

void free_couple(COUPLE *c)
{
  if (c==NULL) return;
  free(c);
}

/* Create an empty generation structure pointer */
GENERATION *create_generation()
{
 GENERATION *g=NULL;
  if ((g=(GENERATION *)malloc(sizeof(GENERATION)))==NULL)
  {
	perror("generation");
	return NULL;
  }
  g->id=0;
  g->nbcouples=0;
  g->couples=(int *)NULL;
  g->nbindivs=0;
  g->indivs=(int *)NULL;
  g->nbboxes=0;
  g->boxes=(INDIV_BOX **)NULL;
  return (GENERATION *)g;
}


void free_generation(GENERATION *g)
{
 int i=0;
 if (g==NULL) return;
 if (g->couples!=NULL) free(g->couples);
 if (g->indivs!=NULL) free(g->indivs);
 free(g);
}



PEDIGREE *create_pedigree()
{
 PEDIGREE *p=NULL;
 if ((p= (PEDIGREE *)malloc(sizeof(PEDIGREE)))==NULL) /* Malloc PEDIGREE */
  {
	perror("Malloc New Pedigree");
        return (PEDIGREE *)NULL;
  }
  p->id=0;
  strcpy(p->filename,"");
  p->nbindivs=(int)0;
  p->indivs=(INDIV **)NULL;
  p->nbcouples=(int)0;
  p->couples=(COUPLE **)NULL;
  p->nbgenerations=(int)0;
  p->generations=(GENERATION **)NULL;
  return (PEDIGREE *)p;
}

int get_indiv_generation(PEDIGREE *p, int i)
{
  INDIV *ind=NULL;
  if ((ind=p->indivs[i])==NULL) return -1;
  return (int)(ind->generation);
}

int set_kids_ratio(PEDIGREE *p, int mother,int father,int *done)
{
int i;
int j;
double DY=0.0;
double minpos=0.0;	/* Left Parent */
double ypos=0.0;
double ratio=0.0;
int nbsingle=0;
int nbmarried=0;
int count=0;
  if (p->indivs[mother]->x < p->indivs[father]->x)
	minpos=p->indivs[mother]->x+p->indivs[mother]->ratio*(double)0.5;	/* Take the most left X position */
  else		/* Take the most left X position */
	minpos=p->indivs[father]->x+p->indivs[father]->ratio*(double)0.5;
#if 0
  if (p->indivs[mother]->x < p->indivs[father]->x)
	minpos=p->indivs[mother]->x;	/* Take the most left X position */
  else
	minpos=p->indivs[father]->x;	/* Take the most left X position */
#endif
  for (i=0; i<p->nbindivs; i++)
  {
    if ((p->indivs[i]->father==father)&&
	(p->indivs[i]->mother==mother))
    {
	if (p->indivs[i]->spouse>0) nbmarried++;
	else nbsingle++;
    }
  }
#if 0
  printf("COUPLE (%d,%d) => %d married kids, %d single kids\n", mother+1, father+1,nbmarried, nbsingle);
  printf("STARTPOS=%f\n", minpos);
#endif
  if ((nbsingle==1)&&(nbmarried==0))	/* Only one child */
  {					/* All parents ratio is taken */
    ratio=(double)
#if 0
      (p->indivs[mother]->ratio + p->indivs[father]->ratio) /
#endif
      (p->indivs[mother]->ratio) /
		(double)((double)nbmarried+(double)nbsingle);
  }
  else
  {
    if ((nbsingle==0)&&(nbmarried==1))
    {
      ratio=(double)
          (p->indivs[mother]->ratio) /
		(double)3.0;	/* The first "third" is empty */
#if 0
          (p->indivs[mother]->ratio + p->indivs[father]->ratio) /
#endif
    }
    else
    {
      ratio=(double)
          (p->indivs[mother]->ratio) /
		(double)(2.0*(double)nbmarried+(double)nbsingle);
#if 0
          (p->indivs[mother]->ratio + p->indivs[father]->ratio) /
#endif
    }
  }
	/* Find the kids first */
  DY=(double)((double)1.0/(double)(p->nbgenerations));
#if 0
  printf("RATIO COUPLE (%d,%d)=%f\n",mother,father,ratio);
#endif
  for (i=0; i<p->nbindivs; i++)
  {
    if ((p->indivs[i]->father==father)&&
	(p->indivs[i]->mother==mother))
    {
	if (p->indivs[i]->spouse>0)	/* Married */
	{
	  if (((count==0)&&(p->indivs[i]->sib>=0))||
	      ((count>0)&&(p->indivs[i]->sib>=0)))
	  {
	    p->indivs[i]->ratio=(double)ratio;
            p->indivs[i]->x= minpos + ratio*(float)count;
            p->indivs[i]->y=(double)(p->indivs[i]->generation)+ (double)0.5;
            p->indivs[i]->y=(double)(p->indivs[i]->y)*(double)DY;
            count++;
	    p->indivs[p->indivs[i]->spouse]->ratio=(double)ratio;
            p->indivs[p->indivs[i]->spouse]->x= minpos + ratio*(double)count;
            p->indivs[p->indivs[i]->spouse]->y=
             (double)(p->indivs[p->indivs[i]->spouse]->generation)+ (double)0.5;
            p->indivs[p->indivs[i]->spouse]->y=
                        (double)(p->indivs[p->indivs[i]->spouse]->y)*(double)DY;
            count++;
	  }
	  else
	  {
	    p->indivs[p->indivs[i]->spouse]->ratio=(double)ratio;
            p->indivs[p->indivs[i]->spouse]->x= minpos + ratio*(double)count;
            p->indivs[p->indivs[i]->spouse]->y=
	     (double)(p->indivs[p->indivs[i]->spouse]->generation)+ (double)0.5;
	    p->indivs[p->indivs[i]->spouse]->y=
			(double)(p->indivs[p->indivs[i]->spouse]->y)*(double)DY;
	    count++;
    	    p->indivs[i]->ratio=(double)ratio;
            p->indivs[i]->x= minpos + ratio*(float)count;
            p->indivs[i]->y=(double)(p->indivs[i]->generation)+ (double)0.5;
            p->indivs[i]->y=(double)(p->indivs[i]->y)*(double)DY;
            count++;
	  }
	}
	else
	{
	  p->indivs[i]->ratio=(double)ratio;
          p->indivs[i]->x= minpos +ratio*(double)count;
          p->indivs[i]->y=(double)(p->indivs[i]->generation)+ (double)0.5;
	  p->indivs[i]->y=(double)(p->indivs[i]->y)*(double)DY;
	  count++;
	}
    }
  }
}

PEDIGREE *load_pedigree(char *filename, char onlyload)
{
 PEDIGREE *p=NULL;
 int *iptr=NULL;
 char finished=0;
 int nbcouples=0;
 int nbckids=0;
 int nbkids=0;
 COUPLE *c=NULL;
 COUPLE **couples;
 GENERATION *gen=NULL;
 GENERATION *gen0=NULL;
 INDIV *ind=NULL;
 INDIV *ind1=NULL;
 INDIV *ind2=NULL;
 FILE *fd=NULL;
 int *done=NULL;
 int nbdone=0;
 int fag=0, mog=0;	/* Father Generation, Mother Generation */
 char found=0;
 char line[MAX_FILENAME]={""};
 time_t Sometime;
 char strtmp[MAX_FILENAME]={""};
 int id,sx,fa,mo,sp,kd,si,pr,ph;
 int l=0;
 int i=0, j=0, k=0;
 int par0;
 int par1;

  if ((fd=fopen(filename,"r"))==NULL)	/* Open & Check File */
  {
	perror(filename);
	return (PEDIGREE *)NULL;
  }
  if ((p=(PEDIGREE *)create_pedigree())==NULL)
  {
	fclose(fd);
	perror(filename);
	return (PEDIGREE *)NULL;
  }
  strcpy(p->filename,filename);

  Sometime=time(&Sometime);				/* Init PEDIGREE */
  p->id=Sometime;
  l=0;
  while (!feof(fd))					/* Process file */
  {
     if (fgets(line,MAX_FILENAME,fd)!=NULL)
     {
        l++;
	if (sscanf(line,"%d %d %d %d %d %d %d %d %d",
		&id, &sx,&mo, &fa, &sp, &kd, &si, &pr, &ph) !=9)
	{
#if 0
	    printf("Can't read values on line: %d in %s.\n", l,filename);
	    fclose(fd);
#endif
   	    if (p!=NULL) free(p);
	    return (PEDIGREE *)NULL;
	}						/* Add INDIV */
	if (p->indivs==NULL)	/* Malloc or realloc indivs */
	    p->indivs=(INDIV **)malloc((p->nbindivs+1)*sizeof(INDIV *));
	else
	    p->indivs=(INDIV **)
			realloc(p->indivs, (p->nbindivs+1)*sizeof(INDIV *));
	if ((p->indivs[p->nbindivs]=(INDIV *)create_indiv())==NULL)
	{
	    perror("Create Indiv");
	    exit(1);
	}
	ind=(INDIV *)(p->indivs[p->nbindivs]);
	p->nbindivs = (int)(p->nbindivs) + 1;
  	ind->id=id-1; ind->sex=sx; ind->father=fa-1; ind->mother=mo-1;
	ind->spouse=sp-1; ind->kid=kd-1;ind->sib=si-1; ind->proband=pr;
	ind->pheno=ph; ind->generation= -1;
#if 0
	printf("ind->id=%d\tind->spouse=%d\n", ind->id+1,ind->spouse+1);
#endif
     }
  }
  fclose(fd);
  if (onlyload)
	return (PEDIGREE *)p;
#if 0
  printf("%d individual found in %s.\n",p->nbindivs, p->filename);
#endif

  done=(int *)malloc(p->nbindivs*sizeof(int));	/* Set the pedigree->nbindivs */
  for (i=0;i<p->nbindivs;i++)
	done[i]=0;				/* Mark them not analysed */
						/* Create generation 0 */
  
	/* NOW FIND THE GENERATION 0 COUPLES */
  done=(int *)malloc(p->nbindivs*sizeof(int));	/* Set the pedigree->nbindivs */
  for (i=0; i<p->nbindivs; i++)
	done[i]=0;
  nbdone=0;
  for (i=0; i<p->nbindivs; i++)
  {
     ind=(INDIV *)(p->indivs[i]);
     if ((!done[i])&&(ind->spouse>=0))	/* Coupled Individuals */
     {
	ind1=p->indivs[ind->spouse];
	if ((ind->father<0)&&(ind->mother<0)&&(ind1->father<0)&&(ind1->mother<0))
	{
	   ind->generation=(int)0;
	   done[ind->id]=1;
	   nbdone++;
	   ind->ratio=(double)0.5;
	   if (ind->sex==0)
	   {
	     ind->x=0.0;
	     ind1->x=0.5;
	   }
	   else
	   {
	     ind1->x=0.0;
	     ind->x=0.5;
 	   }
	   ind1->generation=(int)0;
	   done[ind1->id]=1;
	   ind1->ratio=(double)0.5;
	   nbdone++;
#if 0
	   printf("Content-Type: text/html\r\n\r\n");
  	   printf("COUPLE GENERATION0 (%d,%d)<BR>",ind->id+1, ind1->id+1);
#endif
	}
     }
  }


	/* SETS THE OTHER COUPLED INDIVIDUAL GENERATION NUMBER */
  j=0;
  while (nbdone<p->nbindivs)
  {
#if 0
    printf("pass%d\n",j++);
#endif
    for (i=0; i<p->nbindivs; i++)
    {
      if (!done[i])
      {
        ind=p->indivs[i];
        if ((ind->father>=0)&&(ind->mother>=0))
	{
	  fa=ind->father;
	  if (done[fa])
	  {
		ind->generation=(int)(p->indivs[fa]->generation)+1;
		done[i]=1;
		nbdone++;
	        if (ind->spouse>=0)
	        {
		  p->indivs[ind->spouse]->generation=ind->generation;
		  done[ind->spouse]=1;
		  nbdone++;
		}
	  }
        }
     }
   }
 }
 for (i=0; i<p->nbindivs;i++)
   if (p->indivs[i]->generation>p->nbgenerations)
	p->nbgenerations=p->indivs[i]->generation;
 p->nbgenerations=p->nbgenerations+1;

 for (i=0; i<p->nbindivs; i++)
	done[i]=0;
 nbdone=0;

  /* LETS MAKE THE SIBLINGS RATIO FOR GENERATION 1 */

  /* NOW LETS CALCULATE THE RATIOS FOR KIDS ! */
 for (i=0; i<p->nbgenerations; i++)	/* FOR EACH GENERATION */
 {
	/* FIND COUPLES WITH BOTH RATIO>0.0 in CURRENT GENERATION */
#if 0
   printf("********GENERATION %d RATIOS*******\n",i);  
#endif
   for (j=0; j<p->nbindivs; j++)
   {
     if (!done[j])	/* RATIO DONE */
     {
	if (p->indivs[j]->generation==i)	/* IN CURRENT GENERATION ONLY */
	{
#if 0
   	  printf("********TESTING %d RATIOS*******\n",i);  
#endif
	  if (p->indivs[j]->spouse>=0)
	  {
	    par0=j;
	    par1=p->indivs[j]->spouse;
	    if ((p->indivs[par0]->ratio>0.0)&& /* PARENTS RATIO MUST BE KNOWN */
	        (p->indivs[par1]->ratio>0.0))
	    {
	      if (p->indivs[par0]->sex==0)
	      {
	    	set_kids_ratio(p, par0,par1,done);
	      }
	      else
	      {
		set_kids_ratio(p,par1,par0,done);
	      }
            }
#if 0
	    else
		printf("Warning: parents (%d, %d) have no ratio in generation %d\n", par0, par1, i-1);
#endif
	    done[par0]=1; done[par1]=1;
	  }
	}
     }
   }
 }

 return (PEDIGREE *)p;
}
