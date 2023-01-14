#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <math.h>

#include "ped.h"

#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"

#define RADIUS_CIRCLE	5
#define SIDE_SQUARE	2*RADIUS_CIRCLE

/* LARGE_MARGIN IS USED TO START OF WITH POSITIVE OFFSETS */
#define LARGE_MARGIN	10000

/* If no method is applied, each parent is above any of its descendants, 
   including the spouse of their descendants... */

/* This method is for fixed width (4*RADIUS_CIRCLE) between any parents... */
#define CENTER_COUPLE_METHOD1 0

/* Obsolete
#define CENTER_COUPLE_METHOD2 0

/* This one is the most fancy and should be used in production */
#define CENTER_COUPLE_METHOD3 1

/* That draws 3 lines between the couples... I find it ugly */
#define DRAW_COUPLE_3_LINES	0

#define Black	0
#define Cyan	1
#define Red 2
#define Blue 3
#define Green 4

#define NBCOLORS 16
static int *colors=NULL;
 
static int black;
static int blue;
static int real_blue;
static int sky_blue;
static int red;
static int brown;
static int light_green;
static int light_blue;
static int gray;
static int light_orange;
static int purple;
static int orange;
static int green;
static int pink;
static int light_brown;
static int white;

static int Draw_Phenotypes=0;

/* M_PI DIVIDER FOR FILLED CIRCLES */
#define NB_CIRCLE_DIVISION	40.0

void draw_circle(gdImagePtr im, int cx, int cy, int radius, char filled, int color)
{
 int x=0, y=0;
 int i=0;
 double dangle;
 double alpha;
 int rad;
 if (filled)
 {
   for (rad=1; rad<=radius; rad++)
   {
      /*dangle=360.0/120.0;*/
      dangle=(double)M_PI/(double)NB_CIRCLE_DIVISION;
      for (i=0; i<2*NB_CIRCLE_DIVISION; i++)
      {
        alpha=(float)(dangle*(float)i);
        x=cx+(int)((double)rad*(double)cos(alpha));
        y=cy+(int)((double)rad*(double)sin(alpha));
        gdImageSetPixel(im, x, y, color);
      }
   }
 }
 else
 {
      rad=radius;
      dangle=360.0/(float)(4*rad);
      for (i=0; i<4*rad; i++)
      {
        alpha=(float)(dangle*(float)i);
        x=cx+(int)((float)rad*(float)cos(alpha));
        y=cy+(int)((float)rad*(float)sin(alpha));
        gdImageSetPixel(im, x, y, color);
      }
 }
}

/* The way the software places individuals implies that any descendant->x
position is included between mostleft_parent->x and mostright_parent->x  */
/* This function will very quickly test if an individual (individ) is a
descendant of a couple of parents in the pedigree (par0 and par1) */
int is_a_descendant(PEDIGREE *p, int par0, int individ)
{
int mostleft=-1;
int mostright=-1;
int par1=-1;
  if ( (par0<0)|| (par0>=p->nbindivs)) return -1; /* No such indiv */
  if (p->indivs[par0]->spouse<0) return 0; /* One couples have descendants */
  par1=p->indivs[par0]->spouse;	/* Here is the other parent */
	/* Find the most left individual in the parents */
  if (p->indivs[par0]->x<p->indivs[par1]->x) { mostleft=par0; mostright=par1; }
  else { mostleft=par1; mostright=par0; }
	/* individual's generation must be smaller than parents' */
  if ((double)(p->indivs[individ]->generation) >
      					(double)(p->indivs[par0]->generation))
  {	/* individual x position must be within the parents x positions */
    if (((double)(p->indivs[mostleft]->x) <= (double)(p->indivs[individ]->x))&&
        ((double)(p->indivs[individ]->x) <= (double)(p->indivs[mostright]->x)))
	return 1; /* Individual is a descendant */
  }
  return 0;	/* Individual is not a descendant */
}

/* This function is used to obtain the position of the most left descendant
who happens to be in the generation right below a couple.
/* par0 SHOULD BE THE MOST LEFT PARENT ! */
int get_couple_mostleftx_kid(PEDIGREE *p, int par0, int par1)
{
INDIV *ind=NULL;
int i,j;
int mother, father;
int mostleftx;
int generation;

	/* Find the mother of the 2 */
  if (p->indivs[par0]->sex==0)
  {
	mother=par0; father=par1;	/* Parent 0 is mother */
  }
  else
  {
 	mother=par1; father=par0;	/* Parent 1 is mother */
  }

  mostleftx=
	(int)(p->indivs[par1]->xpic);/* Start on the right of the couple */
  for (i=0; i<p->nbindivs; i++)	/* Update the most left for this couple */
  {
    if (p->indivs[i]->generation == (p->indivs[par0]->generation+1))
    {
	ind=p->indivs[i];
	if ((ind->father==father)&&
	    (ind->mother==mother))
	{
	  if (p->indivs[i]->xpic<mostleftx)
	  {
		mostleftx=p->indivs[i]->xpic;  /* i becomes the most left now */
	  }
	}
    }
  }
  return (int)mostleftx;
}

int get_couple_mostrightx_kid(PEDIGREE *p, int par0, int par1)
{
INDIV *ind=NULL;
int i,j;
int mother, father;
int mostrightx;
int generation;

        /* Find the mother of the 2 */
  if (p->indivs[par0]->sex==0)
  {
        mother=par0; father=par1;       /* Parent 0 is mother */
  }
  else
  {
        mother=par1; father=par0;       /* Parent 1 is mother */
  }

  mostrightx=
        (int)(p->indivs[par0]->xpic);/* Start on the right of the couple */
  for (i=0; i<p->nbindivs; i++) /* Update the most left for this couple */
  {
    if (p->indivs[i]->generation == (p->indivs[par0]->generation+1))
    {
        ind=p->indivs[i];
        if ((ind->father==father)&&
            (ind->mother==mother))
        { 
          if (p->indivs[i]->xpic>mostrightx)
          {     
                mostrightx=p->indivs[i]->xpic;  /* i becomes the most left now */
          }
        }
    }
  }
  return (int)mostrightx;
}




/* The translation occurs for latest generation couple first (!done individuals)
   and translate the kids towards the most left using the minimum possible */
int translate_couple_kids(PEDIGREE *p, int par0, int par1,int transx)
{
INDIV *ind=NULL;
int i;

  for (i=0; i<p->nbindivs; i++) /* Update the most right for this couple */
  {
    if (p->indivs[i]->generation > p->indivs[par0]->generation)
    {
      if (is_a_descendant(p,par0,i)>0)
      {
        ind=p->indivs[i];
	ind->xpic=ind->xpic-transx;
      } 
    }
  }
  p->indivs[par1]->xpic=p->indivs[par1]->xpic-transx;
  p->indivs[par0]->xpic=p->indivs[par0]->xpic-transx;
  i=p->indivs[par1]->xpic;
  return (int)i;
}



static void ini_colors(gdImagePtr im)
{
	colors=(int *)malloc(NBCOLORS*sizeof(int));
        white = gdImageColorAllocate(im, 255, 255, 255);
	colors[0]=white;
        black = gdImageColorAllocate(im, 0, 0, 0);
	colors[1]=black;
        red = gdImageColorAllocate(im, 220, 0, 0);
	colors[2]=red;
        green = gdImageColorAllocate(im, 0, 122, 0);
	colors[3]=green;
        blue = gdImageColorAllocate(im, 0, 0, 150);
	colors[4]=blue;
        pink = gdImageColorAllocate(im, 255, 100, 200);
	colors[5]=pink;
        light_green = gdImageColorAllocate(im, 157, 222, 143);
	colors[6]=light_green;
        light_blue = gdImageColorAllocate(im, 50, 162, 175);
	colors[7]=light_blue;
        brown= gdImageColorAllocate(im, 130, 41, 0);
	colors[8]=brown;
        light_brown= gdImageColorAllocate(im, 130, 81, 58);
	colors[9]=light_brown;
        orange= gdImageColorAllocate(im, 255, 102, 0);
	colors[10]=orange;
        sky_blue = gdImageColorAllocate(im, 100, 200, 255);
	colors[11]=sky_blue;
        light_orange= gdImageColorAllocate(im, 255, 188, 71);
	colors[12]=light_orange;
        real_blue = gdImageColorAllocate(im, 0, 0, 255);
	colors[13]=real_blue;
        gray= gdImageColorAllocate(im, 153, 153, 153);
	colors[14]=gray;
        purple= gdImageColorAllocate(im, 150, 0, 255);
	colors[15]=purple;
}


int imx=600;
int imy=500;

float Xratio=1.0;
float Yratio=1.0;

int minimum_x=20;
int minimum_y=20;


int set_kids_width(PEDIGREE *p, int mother, int father, int next_x, int *done)
{
 INDIV *ind=NULL;
 int count=0;
 int i=0;
 for (i=0; i<p->nbindivs; i++)
 {
   ind=p->indivs[i];
   if ((ind->mother==mother) && (ind->father==father))
   {
      count++;
      ind->x=next_x+count*MIN_INDIV_WIDTH*2;
      ind->w=MIN_INDIV_WIDTH*2;
   }
 }
#if VERBOSE
 printf("MOTHER%d-FATHER%d=%d Kids\n",mother, father, count);
#endif
 return count;
}

int compute_indivs(PEDIGREE *p, int *done)
{
GENERATION *gen=NULL;
INDIV *par0=NULL;
INDIV *par1=NULL;
INDIV *ind=NULL;
int kids_w=0;
int next_x=0;
int i=0;
              /* PLACE GENERATION 1 COUPLES */
 gen=p->generations[1];
 next_x=0;
 for (i=0; i<gen->nbindivs; i++)
 {
   if (!done[gen->indivs[i]])
   {
     par0=p->indivs[gen->indivs[i]];
     if (par0->spouse>=0)        /* We have a couple */
     {
	par1= p->indivs[par0->spouse];
        done[gen->indivs[i]]=1;
        done[par0->spouse]=1;
		/* CHECK WIDTH OF KIDS */
     }
   }
 }
}

#define MINBOX_SIZE	20.0
#define TOPBORDER	20.0
#define LEFTBORDER	20.0

gdImagePtr draw_indivs(PEDIGREE *p)
{
INDIV *saveind=NULL;
char logfile[255]={""};
int mother=-1;
int father=-1;
int updatepar1=0;
int left, right;
int digitlen=0;
int c=0;
int c2=0;
int c3=0;
int mo,fa;
gdImagePtr im=NULL;
GENERATION *gen=NULL;
double transx=0;
int transxpic=0;
int generation;
int i,j,k;
char found=0;
int x=0, y=0;
int x1=0;
double xx=0;
double xx1=0;
double xf,yf;
int dx=0; int dy=0;
char strtmp[255]={""};
double ratio=1.0;
double multx=1.0;
int maxx=0;
int minx=0;
int maxy=0;
double xfe=0.0;
double dY=0.0;
int *done=NULL;
int *done2=NULL;
INDIV *ind=NULL;
double d=0.0;

INDIV *spouseind=NULL;
INDIV *nextsibind=NULL;
int mostleft;
double mostleftx;
INDIV *mostleftsib=NULL;
INDIV *mostleftspouse=NULL;
INDIV *lastleftind=NULL;
int nextposx;
int par0,par1;	/* par0 is always most left */
double par0x;
double par1x;
 FILE *fd=NULL;
  
  chdir("/tmp");
  done=(int *)malloc(p->nbindivs*sizeof(int));  /* Set the pedigree->nbindivs */
  done2=(int *)malloc(p->nbindivs*sizeof(int));  /* Set the pedigree->nbindivs */
  for (i=0;i<p->nbindivs;i++)
  {
        done[i]=0;                              /* Mark them not analysed */
	done2[i]=0;
  }
                                                 /* Create generation 0 */

	/* Find the smallest ratio */
  for (i=0; i<p->nbindivs; i++)
  {
     if (p->indivs[i]->ratio<ratio)
     {
	ratio=(double)(p->indivs[i]->ratio);
     }
  }

  multx=(double)MINBOX_SIZE/(double)ratio;
#if VERBOSE
  printf("DRAWING INDIVIDUALS (smallest ratio=%f)\n",ratio);
  printf("DRAWING INDIVIDUALS (smallest ratio=%f)\n",ratio);
#endif
  maxx=0;
  maxy=(p->nbgenerations+1)*2*MINBOX_SIZE;
  for (i=0; i<p->nbindivs; i++)
  {
	xf=p->indivs[i]->x;
	xf=xf+((double)(p->indivs[i]->ratio)/(double)2.0);
	xf=(double)xf*(double)multx;
	if ((int)xf>maxx) maxx=(int)xf;
	p->indivs[i]->x=xf; /*+(double)LEFTBORDER;*/
	yf=p->indivs[i]->y*(double)maxy;
	p->indivs[i]->y=yf+(double)TOPBORDER;
#if VERBOSE
	printf("Indiv[%d] => (%f, %f)\n", i+1,xf,yf);
#endif
  }

	/* Ratio position x and y are now set for all individuals */

/* THIS IS GOING TO BE THE MAXIMUM Y SIZE OF THE FINAL PICTURE */
  maxy= (int)((double)TOPBORDER+(double)2.0+	/* TOPBORDER = BOTBORDER */
	(double)((double)16.0*(double)RADIUS_CIRCLE*(double)(p->nbgenerations)));
  dY= (double)maxy/(double)(p->nbgenerations)/(double)2.0; /* What is the current dY ratio */


/*******************************************************************************/
/* -> COMPRESSION OF EXTRA BLANK SPACE					       */
/* WE NEED TO COMPUTE THE XPIC POSITION IN THE IMAGE FROM THE X RATIOS         */
/*******************************************************************************/

  /* THE FIRST THING WE DO IS TO SET INITIAL XPIC VALUES FROM X VALUES */
#if LOGFILE
  strcpy(logfile,"/home/cperson/tmp/testit");
  fd=fopen(logfile,"w");
#endif
  
  /* LETS UPDATE THE XPIC FOR !DONE INDIVIDUALS (YPIC=(int)y) */
  for (i=0; i<p->nbindivs; i++)
  {
	p->indivs[i]->xpic=(int)(p->indivs[i]->x)+ LARGE_MARGIN;
	p->indivs[i]->ypic=(int)((double)(p->indivs[i]->generation)*(double)dY)+(int)((double)dY/(double)2.0)+(double)TOPBORDER;
	done[i]=0;
  }

  /* Now check if some of them need to be inverted */
  for (i=2; i<p->nbindivs; i++)
  {
    ind=p->indivs[i];
    if (ind->father>=0)
    if ((ind->spouse>=0)&&(ind->sib<0)&&(p->indivs[ind->father]->kid!=i))
    {
#if LOGFILE
	fprintf(fd,"SWITCHING INDIVIDUALS %d (%d) & %d(%d)\n",i+1,ind->xpic,ind->spouse+1,p->indivs[ind->spouse]->xpic);
#endif
	j=p->indivs[ind->spouse]->xpic;
	d=p->indivs[ind->spouse]->x;
	p->indivs[ind->spouse]->xpic=p->indivs[i]->xpic;
	p->indivs[ind->spouse]->x=p->indivs[i]->x;
	p->indivs[i]->xpic=j;
	p->indivs[i]->x=d;
    }
  }


  /* INDIVIDUALS ARE PROCESSED FROM THE LAST GENERATION TO THE FIRST */
  for (generation=p->nbgenerations-1; generation>=1; generation--)
  {
	/* WE NEED VARIABLES TO STORE THE LAST MOSTLEFT INDIVIDUAL */
	/* FIND THE NEXT, !done,  MOST LEFT INDIV IN THE GENERATION */
	/* AND GIVE HIM XPIC=FATHER(X) */

    if (found)		/* LETS MAKE ANOTHER ROUND TO CHECK OTHER SIBLINGS */
	generation++;
    found=0;
    /* ANY INDIVIDUALS IN ABOVE GENERATIONS CAN NOT PROVIDE HIS XPIC YET */
    for (i=0; (i<p->nbindivs)&&(!found); i++)
    {
	  if (p->indivs[i]->generation==generation) /* In CURRENT generation */
	  {  /* INDIV MUST NOT BE DONE ALREADY */
	     if (!done[i])	/* Individual is not locked */
	     {	/* THIS ONE IS MOST LEFT THAN THE CURRENT MOSTLEFT */
		if ((p->indivs[i]->mother>=0)&&(p->indivs[i]->father>=0))
		{
		   found=1;
		   mostleft=i; /* IT WILL HAVE ITS MOSTLEFT PARENT->X POSITION */
		   mostleftsib=p->indivs[mostleft];
		   mother=mostleftsib->mother;
		   father=mostleftsib->father;
#if LOGFILE
		   fprintf(fd,"PROCESSING NEW CHILD %d WITH PARENTS(%d,%d)\n",
			mostleft+1,
			mother+1,father+1);
#endif
		}
	     }
	  }
    }
    if (found)	/* THERE ARE STILL SIBLINGS TO PROCESS IN THIS GENERATION */
    { /* WE NEED TO GET THE PARENTS OF THIS INDIVIDUAL (OR ITS SPOUSE'S) */
	if (mostleftsib->mother>=0)
 	   par0=mostleftsib->mother;
	else
	   par0=p->indivs[mostleftsib->spouse]->mother;
	par1=p->indivs[par0]->spouse;	/* Its mother's spouse */


	if (p->indivs[par0]->xpic>p->indivs[par1]->xpic)
	{	/* Invert par0 and par1 */
	        j=par0; par0=par1; par1=j; /* FATHER IS THE MOST LEFT PARENT */
	}
#if LOGFILE
        fprintf(fd,"\nNEW PARENTS (par0=%d, par1=%d)\n",par0+1,par1+1);
#endif
		/* WE GOT THE MOST LEFT NEXT SIBLING TO DEAL WITH */
		/* FOR THIS ROUND OF SIBLINGS TRANSLATION */
	
	/* NOW WE LOCK THIS NEW FOUND MOSTLEFT */
	/* Test if this guy has any siblings at all */

	nextposx=p->indivs[par0]->xpic;
#if LOGFILE
	fprintf(fd,">>>CURRENT NEXTPOS=%d (PARENT0)\n",nextposx);
	fprintf(fd,"NEW SIBLING INDIV %d NEXTPOSX=%d\n",mostleftsib->id+1,nextposx);
#endif
	/**********************************************************************/
	/* IF THE CURRENT MOSTLEFTSIB HAS NO SIBLING THE CENTERING IS SPECIAL */
	/**********************************************************************/
	if (mostleftsib->sib<0)
	{
#if LOGFILE
	    fprintf(fd,"\tSIBLING %d (POS=%d) HAS NO OTHER SIBLING !\n", mostleft+1,nextposx);
#endif
	    if ((mostleftsib->spouse>=0)&&(mostleftsib->father>=0))
	    {
		spouseind=p->indivs[mostleftsib->spouse];
	        /* THIS FUNCTION ALSO UPDATE par0 and par1 */
#if LOGFILE
                fprintf(fd,"\t\tLAST SIBLING %d (POS=%d) IS MARRIED TO %d (%d).\n", mostleft+1, nextposx, mostleftsib->spouse, spouseind->xpic);
		fprintf(fd,">>>CURRENT NEXTPOS=%d (SIB=0 SPOUSE=%d) BEFORE TRANS\n",nextposx, mostleftsib->spouse);
#endif
		if (mostleftsib->kid>0)
		{
                  transxpic=spouseind->xpic-p->indivs[par0]->xpic;
                  nextposx=translate_couple_kids(p, mostleft,
			mostleftsib->spouse,
			transxpic);
		}
	        else
		{
		  nextposx=p->indivs[par0]->xpic;
		  spouseind->xpic=nextposx;
                  nextposx=(int)((double)nextposx+(double)4.0*(double)RADIUS_CIRCLE);
		  mostleftsib->xpic=nextposx; /* THE MIDDLE BELOW PARENTS */
		}
#if LOGFILE
		fprintf(fd,">>>CURRENT NEXTPOS=%d (SIB=0 SPOUSE=%d) AFTER TRANS\n",nextposx, mostleftsib->spouse);
#endif
		
#if CENTER_COUPLE_METHOD3
		nextposx= mostleftsib->xpic+4*(int)RADIUS_CIRCLE; 
#if CENTER_COUPLE_METHOD1
		p->indivs[par0]->xpic=mostleftsib->xpic-4*(int)RADIUS_CIRCLE;
#endif
		p->indivs[par1]->xpic=nextposx;
#endif
#if CENTER_COUPLE_METHOD1
		/* For this one, method 1 needs to be on */
		nextposx= 2*mostleftsib->xpic-spouseind->xpic; 
		p->indivs[par1]->xpic=nextposx;	
#endif
                updatepar1=0;
		done[mostleft]=1; done[mostleftsib->spouse]=1;
	    }
	    else
	    {
	      nextposx=nextposx+4*(int)RADIUS_CIRCLE;
#if LOGFILE
	      fprintf(fd,">>>CURRENT NEXTPOS=%d (SIB=0 SPOUSE=0)\n",nextposx);
#endif
              p->indivs[par1]->xpic= nextposx;
	      mostleftsib->xpic=(int)((double)((double)(p->indivs[par0]->xpic)
	    		            +(double)(p->indivs[par1]->xpic))
			   	   /(double)2.0);
	      done[mostleft]=1;
#if LOGFILE
	      fprintf(fd," *** LAST SIBLING %d (X%d) FOR PARENTS(%d,%d) DONE\n",mostleft+1,mostleftsib->xpic,par0+1,par1+1);
#endif
	    }
	    done2[par0]=1;	/* These will need special recentering */
	    done2[par1]=1;
						/* SIBLINGS FINISHED ! */
        }
	else /* THE FIRST SIBLING IS ALWAYS GOING TO BE BELOW PAR0 */
	/**********************************************************************/
	/* THE CURRENT MOSTLEFTSIB HAS OTHER SIBLINGS COMING                  */
	/**********************************************************************/
        {
#if BUG
/* NO!! THAT WAS A BAD BUG -> DONT WORK FOR COUPLE */
            nextposx=p->indivs[par0]->xpic;
#endif
            nextposx=p->indivs[par0]->xpic;

	    /* That should be in the loop of couple or not */
		/* WHILE STILL POSSIBLE, */
	    updatepar1=1;
	    
	    while ((mostleftsib->father>=0)&&	     /* GET NEXT SIB OR SPOUSE */
		   ((mostleftsib->spouse>=0)||(mostleftsib->sib>=0)))
	    {
#if LOGFILE
	    	fprintf(fd,"NEW WHILE ROUND for INDIV %d NEXTPOSX=%d\n",mostleftsib->id+1,nextposx);
#endif
		/* CHECK IF THERE ARE MORE SIBLING */
		if (mostleftsib->spouse>=0)	/* HERE IS A COUPLE !! */
		{					/* just translate ! */
		   spouseind=p->indivs[mostleftsib->spouse];
		   if (mostleftsib->kid<0)	/* PARENTS AWAIT KID */
		   {			/* -> FIXED SIZE BETWEEN PARENTS */
		      mostleftsib->xpic=nextposx;
	  	      nextposx= (int)nextposx+
			   + (double)4.0*(double)RADIUS_CIRCLE;
		      p->indivs[mostleftsib->spouse]->xpic=nextposx;
	  	      nextposx= (int)(
			     (double)(mostleftsib->xpic)
			   + (double)8.0*(double)RADIUS_CIRCLE);
		      p->indivs[par1]->xpic=nextposx;
		      updatepar1=0;
#if LOGFILE
		      fprintf(fd,"FOUND NEXT SIBLING %d (X%d) WITHOUT CHILD FROM COUPLE %d,%d spouse->%d sibling->%d\n",mostleft+1, p->indivs[mostleft]->xpic, par0+1,par1+1,mostleftsib->spouse+1,mostleftsib->sib+1);
#endif
		      done2[mostleft]=1;  /* Special recentering for these */
		      done2[mostleftsib->spouse]=1;
		   }
		   else	/* SOME KIDS ARE PRESENT */
		   {
			/* THIS FUNCTION ALSO UPDATE par0 and par1 */
#if LOGFILE
		      fprintf(fd,"FOUND NEXT SIBLING %d (X%d) WITHOUT CHILD FROM COUPLE %d,%d spouse->%d sibling->%d\n",mostleft+1, p->indivs[mostleft]->xpic, par0+1,par1+1,mostleftsib->spouse+1,mostleftsib->sib+1);
#endif
		      	
#if 0
		      if (mostleftsib->sib<0)
		      {
			done2[mostleft]=1;
			done2[mostleftsib->spouse]=1;
		      }
#endif
		        transxpic= mostleftsib->xpic- nextposx;
		      if (mostleftsib->xpic<p->indivs[mostleftsib->spouse]->xpic)
		      	nextposx=translate_couple_kids(p, mostleft, mostleftsib->spouse, transxpic);
		      else
		      	nextposx=translate_couple_kids(p, mostleftsib->spouse,mostleft, transxpic);
		      nextposx=(int)((double)nextposx+(double)4.0*(double)RADIUS_CIRCLE);
		      updatepar1=0;
		   }
		   done[mostleft]=1;
		   done[mostleftsib->spouse]=1;
#if LOGFILE
	    	   fprintf(fd,"**** (NEXTPOSX=%d) COUPLE %d,%d DONE next sibling->%d\n",nextposx,mostleft+1, mostleftsib->spouse+1,mostleftsib->sib+1);
#endif
		   if (mostleftsib->sib>=0)
                   {
                      mostleft=mostleftsib->sib;
                      mostleftsib=p->indivs[mostleft];
		      updatepar1=0;
                   }
		   else
		   {
			
		       mostleft=mostleftsib->spouse;
		       mostleftsib=p->indivs[mostleft];
		       updatepar1=1;
		    /*   mostleftsib->xpic=nextposx;*/
		   }
		}
		else
		{
		  if (mostleftsib->sib>=0)
		  {
		      mostleftsib->xpic=nextposx;
		      done[mostleft]=1;
#if LOGFILE
	    	      fprintf(fd,"**** (NEXTPOSX=%d) SIBLING %d DONE next sibling->%d\n",nextposx,mostleft+1, mostleftsib->sib+1);
	   	      fprintf(fd,"%d(%d)\t%d\t%d [%d]\n",mostleft+1, p->indivs[mostleft]->generation, p->indivs[mostleft]->xpic, p->indivs[mostleft]->ypic,nextposx);
#endif
	  	      nextposx= nextposx + 4*(int)RADIUS_CIRCLE;
                      mostleft=mostleftsib->sib;
                      mostleftsib=p->indivs[mostleft];
                  }
		}
			/* LAST ONE */
		if ((mostleftsib->sib<0)&&(mostleftsib->spouse<0))
		{
		       mostleftsib->xpic=nextposx;
		       done[mostleft]=1;
            		p->indivs[par1]->xpic=(double)mostleftsib->xpic;
			updatepar1=0;
#if LOGFILE
			fprintf(fd,"FOUND LAST SIBLING %d COUPLE %d,%d ->%d\n",mostleft+1,par0+1,par1+1);
	   		fprintf(fd,"%d(%d)\t%d\t%d [%d]\n",mostleft+1, p->indivs[mostleft]->generation, p->indivs[mostleft]->xpic, p->indivs[mostleft]->ypic,nextposx);
#endif
		}
	    }
            if (updatepar1) p->indivs[par1]->xpic=(double)mostleftsib->xpic;
		/* THE LAST MOSTLEFTSIB PROVIDES ITS XPIC TO PAR1->XPIC*/
	 }
      } /* NO MORE !DONE SIBLINGS IN CURRENT GENERATION -> MAKE GENERATION-1 */
  }

	/* FINISH UP WITH THE TOP COUPLE */
#if 1
  nextposx=(int)((double)2.0*(double)LEFTBORDER);
  transxpic= p->indivs[0]->xpic-nextposx;
		/* THIS FUNCTION ALSO UPDATE par0 and par1 */
  translate_couple_kids(p, 0, 1, transxpic);
#endif

#if 1

	/* RECENTER COUPLES */
  for (i=0; i<p->nbindivs; i++)
  {
	done[i]=0;
  }
  found=0;
#if CENTER_COUPLE_METHOD1
  for (generation=p->nbgenerations; generation>=0; generation--)
  {
    if (found) generation++;
    found=0;
    for (i=0; i<p->nbindivs; i++)
    {
      if (p->indivs[i]->generation==generation)
      {
       if (!done[i])
       {
        if (p->indivs[i]->spouse>=0)
        {
	   c=get_couple_mostleftx_kid(p,i,p->indivs[i]->spouse);
	   c2=get_couple_mostrightx_kid(p,i,p->indivs[i]->spouse);
	   if (c2!=c)
	   {
	     c=(int)((double)c/(double)2.0);
	     p->indivs[i]->xpic=c-(int)((double)2.0*(double)RADIUS_CIRCLE);
	     p->indivs[p->indivs[i]->spouse]->xpic=c+(int)((double)2.0*(double)RADIUS_CIRCLE);
	   }
           done[i]=1; done[p->indivs[i]->spouse]=1;
        }
        else
        done[i]=1;
       }
      }
    }
  }
#endif
#if CENTER_COUPLE_METHOD2
  found=0;
  for (generation=p->nbgenerations; generation>=0; generation--)
  {
    if (found) generation++;
    found=0;
    for (i=0; i<p->nbindivs; i++)
    {
      if (!done[i])
      {
       if (p->indivs[i]->generation==generation)
       {
        found=1;
        if (p->indivs[i]->spouse>=0)
        {
           if (p->indivs[i]->xpic<p->indivs[p->indivs[i]->spouse]->xpic)
           {
                left=i; right=p->indivs[i]->spouse;
           }
           else
           {
                right=i; left=p->indivs[i]->spouse;
           }
           if (p->indivs[right]->xpic-p->indivs[left]->xpic >
                4*(int)RADIUS_CIRCLE+2)
           {
                c=(int)get_couple_mostleftx_kid(p,left,right);
                c2=(int)get_couple_mostrightx_kid(p,left,right);
                if (c!=c2)
                {
                  c3=(int)(((double)c+(double)c2)/(double)2.0);/*CENTER POSITION*/
                  c2=(int)((double)(c2-c)/(double)4.0);
                  p->indivs[left]->xpic= c3-c2;
                  p->indivs[right]->xpic= c3+c2;
                }
#if 1
                else
                {
                  p->indivs[left]->xpic=p->indivs[left]->xpic-4*(int)RADIUS_CIRCLE;
                  p->indivs[right]->xpic=p->indivs[right]->xpic+4*(int)RADIUS_CIRCLE;
                }
#endif
           }
           done[left]=1; done[right]=1;
        }
        else
        done[i]=1;
       }
      }
    }
  }
#endif
/******************************************************************************/
/* THIRD METHOD FOR FINAL RECENTERING OF COUPLE                               */
/* WHEN done2[par]=1, ONLY PAR0 NEEDS TO BE SLIDED TO THE RIGHT, OTHERWISE,   */
/* BOTH PARENTS NEEDS TO BE RECENTERED (I.E THE ONES WITH SINGLE KIDS         */
/******************************************************************************/
#if CENTER_COUPLE_METHOD3
found=0;
  for (generation=p->nbgenerations; generation>=0; generation--)
  {
    if (found) generation++;
    found=0;
    for (i=0; i<p->nbindivs; i++)
    {
      if (!done[i])
      {
       if (p->indivs[i]->generation==generation)
       {
        found=1;
        if (p->indivs[i]->spouse>=0)
        {
            if (p->indivs[i]->xpic<p->indivs[p->indivs[i]->spouse]->xpic)
            {
                left=i; right=p->indivs[i]->spouse;
            }
            else
            {
                right=i; left=p->indivs[i]->spouse;
            }
	    if (!done2[i])
            {
              if (p->indivs[right]->xpic-p->indivs[left]->xpic >
                  4*(int)RADIUS_CIRCLE+2)
              {
#if LOGFILE
	        fprintf(fd,"RECENTERING COUPLE %d,%d\n", left+1, right+1);
#endif
                c=(int)get_couple_mostleftx_kid(p,left,right);
                c2=(int)get_couple_mostrightx_kid(p,left,right);
                if (c!=c2)
                {
                  c3=(int)(((double)c+(double)c2)/(double)2.0);
                  c2=(int)((double)(c2-c)/(double)4.0);
                  p->indivs[left]->xpic= c3-c2;
                  p->indivs[right]->xpic= c3+c2;
                }
	      }
            }
	    else	/* HERE WE JUST SLIDE PAR0 */
	    {
#if LOGFILE
	        fprintf(fd,"SPECIAL RECENTERING COUPLE %d,%d\n", left+1, right+1);
#endif
			/* There should be only one but unsure ... */
		if (p->indivs[left]->kid>=0)
		{
		  saveind=p->indivs[p->indivs[left]->kid];
		  c=saveind->xpic-p->indivs[left]->xpic;
		  c2=p->indivs[right]->xpic-saveind->xpic;
		  if (c>c2)
		  {
		    p->indivs[left]->xpic=saveind->xpic-c2;
		  }
		  else
		  {
		    p->indivs[right]->xpic=saveind->xpic+c2;
		  }
		}
	    }
		if (p->indivs[i]->father>=0)
		{
	          if ((p->indivs[i]->sib<0)&&(p->indivs[p->indivs[i]->father]->kid!=i))
	          {
		   j=p->indivs[i]->xpic;
		   p->indivs[i]->xpic=p->indivs[p->indivs[i]->spouse]->xpic;
		   p->indivs[p->indivs[i]->spouse]->xpic=j;
		  }
	        }
            done[left]=1; done[right]=1;
        }
        else
        done[i]=1;
       }
      }
    }
  }
#endif
	/* GET THE IMAGE GEOMETRY */
  minx=500;
  maxx=0; maxy=0;
#if LOGFILE
    fprintf(fd,"\n\n*** FINAL INDIVIDUAL POSITIONS ***\n");
#endif
  for (i=0; i<p->nbindivs;i++)
  {
    if (p->indivs[i]->xpic<minx)
	minx=p->indivs[i]->xpic;
    if (p->indivs[i]->xpic>maxx)
	maxx=p->indivs[i]->xpic;
    if (p->indivs[i]->ypic>maxy)
	maxy=p->indivs[i]->ypic;
#if LOGFILE
    fprintf(fd," INDIV %d (%d, %d)\n",i+1, p->indivs[i]->xpic, p->indivs[i]->ypic);
#endif
  }
#if LOGFILE
  fprintf(fd,"MINX=%d\tMAXX=%d\tMAXY=%d\n",minx, maxx, maxy);
#endif
  for (i=0; i<p->nbindivs;i++)
  {
    p->indivs[i]->xpic=p->indivs[i]->xpic -(int)minx +(int)LEFTBORDER;
  }
  maxx=maxx-minx+LEFTBORDER;
  maxy=maxy+(int)TOPBORDER;
#if LOGFILE
  fprintf(fd,"Size Image=%dx%d\n", maxx,maxy);
  fclose(fd);
#endif
  
  im = gdImageCreate(maxx+(int)LEFTBORDER,maxy+(int)TOPBORDER);
  ini_colors(im);
	/* DRAW THE LINES FOR COUPLES */
  for (i=0; i<p->nbindivs; i++) done[i]=0;
#if 1
  for (i=0; i<p->nbindivs; i++)
  {
    if (!done[i])
    {
	if (p->indivs[i]->spouse>=0)
	{
	   done[i]=1; done[p->indivs[i]->spouse]=1;
	   if (p->indivs[i]->xpic < p->indivs[p->indivs[i]->spouse]->xpic)
	   {
	      gdImageLine(im,
#if 0
		p->indivs[i]->xpic+(int)RADIUS_CIRCLE,
#endif
		p->indivs[i]->xpic,
		p->indivs[i]->ypic-1,
		p->indivs[p->indivs[i]->spouse]->xpic,
#if 0
		p->indivs[p->indivs[i]->spouse]->xpic-(int)RADIUS_CIRCLE,
#endif
		p->indivs[i]->ypic-1,black);
#if DRAW_COUPLE_3_LINES
	      gdImageLine(im,
		p->indivs[i]->xpic+(int)RADIUS_CIRCLE,
		p->indivs[i]->ypic,
		p->indivs[p->indivs[i]->spouse]->xpic-(int)RADIUS_CIRCLE,
		p->indivs[i]->ypic,black);
#endif
	      gdImageLine(im,
#if 0
		p->indivs[i]->xpic+(int)RADIUS_CIRCLE,
#endif
		p->indivs[i]->xpic,
		p->indivs[i]->ypic+1,
#if 0
		p->indivs[p->indivs[i]->spouse]->xpic-(int)RADIUS_CIRCLE,
#endif
		p->indivs[p->indivs[i]->spouse]->xpic,
		p->indivs[i]->ypic+1,black);
	   }
	   else
	   {
	      gdImageLine(im,
#if 0
		p->indivs[i]->xpic+(int)RADIUS_CIRCLE,
#endif
		p->indivs[i]->xpic,
		p->indivs[i]->ypic-1,
#if 0
		p->indivs[p->indivs[i]->spouse]->xpic-(int)RADIUS_CIRCLE,
#endif
		p->indivs[p->indivs[i]->spouse]->xpic,
		p->indivs[i]->ypic-1,black);
#if DRAW_COUPLE_3_LINES
	      gdImageLine(im,
		p->indivs[p->indivs[i]->spouse]->xpic+(int)RADIUS_CIRCLE,
		p->indivs[i]->ypic,
		p->indivs[i]->xpic-(int)RADIUS_CIRCLE,
		p->indivs[i]->ypic,black);
#endif
	      gdImageLine(im,
		p->indivs[p->indivs[i]->spouse]->xpic+(int)RADIUS_CIRCLE,
		p->indivs[i]->ypic+1,
		p->indivs[i]->xpic-(int)RADIUS_CIRCLE,
		p->indivs[i]->ypic+1,black);
	   }
	   gdImageLine(im,
		(int)(((double)((double)(p->indivs[i]->xpic)+(double)(p->indivs[p->indivs[i]->spouse]->xpic)))/(double)2.0),
		p->indivs[i]->ypic+1,
		(int)(((double)((double)(p->indivs[i]->xpic)+(double)(p->indivs[p->indivs[i]->spouse]->xpic)))/(double)2.0),
		p->indivs[i]->ypic+(int)((double)dY/(double)2.0)+1,black);

           gdImageFilledRectangle(im,
		(int)(((double)((double)(p->indivs[i]->xpic)+(double)(p->indivs[p->indivs[i]->spouse]->xpic)))/(double)2.0)-1,
		p->indivs[i]->ypic+(int)((double)dY/(double)2.0),
		(int)(((double)((double)(p->indivs[i]->xpic)+(double)(p->indivs[p->indivs[i]->spouse]->xpic)))/(double)2.0)+1,
		p->indivs[i]->ypic+(int)((double)dY/(double)2.0)+1,black);
#if 0
	   gdImageLine(im, (int)((double)((double)xx+(double)xx1)/(double)2.0),
			    y,
	   		   (int)((double)((double)xx+(double)xx1)/(double)2.0),
			    y+(int)((double)dY/(double)2.0)+1, black);
#endif
	}
	else
	done[i]=1;
    }
        
  }
#endif

	/* DRAW THE INDIVIDUALS */
  for (i=0; i<p->nbindivs; i++)
  {
	xx=(double)(double)(p->indivs[i]->xpic);
	y=(int)(p->indivs[i]->ypic);
	if ((p->indivs[i]->father>=0)&&
	    (p->indivs[i]->mother>=0))
	{
	   fa=p->indivs[i]->father;
	   mo=p->indivs[i]->mother;
	   gdImageLine(im, (int)xx,
			   y-RADIUS_CIRCLE,
			   (int)xx,
			   y-(int)((double)dY/(double)2.0), black);
	   gdImageLine(im, (int)xx,
                           y-(int)((double)dY/(double)2.0),
			   (int)((double)((double)(p->indivs[fa]->xpic)+(double)(p->indivs[mo]->xpic))/(double)2.0),
                           y-(int)((double)dY/(double)2.0), black);
	}
	switch (p->indivs[i]->sex)
        {
            case 0:
                draw_circle(im, (int)xx, y, RADIUS_CIRCLE, 1, red);
                break;
            case 1:
                dx=(int)RADIUS_CIRCLE;
                gdImageFilledRectangle(im,
			(int)((double)xx-(double)SIDE_SQUARE/(double)2.0),
			y-SIDE_SQUARE/2,
			(int)((double)xx+(double)SIDE_SQUARE/(double)2.0),
			y+SIDE_SQUARE/2, blue);
                break;
        }
	if (Draw_Phenotypes && (p->indivs[i]->pheno>0))
	{
	   gdImageFilledRectangle(im,
                        (int)((double)xx-(double)SIDE_SQUARE/(double)8.0)+1,
                        y-SIDE_SQUARE/8,
                        (int)((double)xx+(double)SIDE_SQUARE/(double)8.0),
                        y+SIDE_SQUARE/8, white);
	}
	sprintf(strtmp,"%d",i+1);
        digitlen=strlen(strtmp);
        if (digitlen>2) 
          gdImageString(im, gdFontTiny, (int)((double)xx-(double)digitlen*(double
)2.0)-1, y+RADIUS_CIRCLE+4, strtmp, black);
        else
          gdImageString(im, gdFontTiny, (int)((double)xx-(double)digitlen*(double
)2.0), y+RADIUS_CIRCLE+4, strtmp, black);
#if 0
	sprintf(strtmp,"%d",i+1);
        gdImageString(im, gdFontTiny, (int)((double)xx-(double)2.0), y+RADIUS_CIRCLE+4, strtmp, black);
#endif
  }
#endif
  return (gdImagePtr)im;
  
}

int gif2pedigree(PEDIGREE *p, char *filename, int draw_pheno)
{
FILE *fd=NULL;
gdImagePtr im = NULL;
int i=0;
int j=0;
int k=0;
unsigned char strtmp[255]={""};
char answer[255]={""};
int lrx,lry,ulx,uly;
int *done=NULL;

   Draw_Phenotypes=draw_pheno;

   if ((fd= fopen(filename, "wb"))==NULL)
   {
     perror(filename);
     exit(1);
   }

   im=draw_indivs(p);
   gdImageGif(im, fd); 
   fclose(fd);
   gdImageDestroy(im);
   im=NULL;
}
