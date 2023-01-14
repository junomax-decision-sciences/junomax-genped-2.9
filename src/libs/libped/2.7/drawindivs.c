
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
double get_couple_mostleftx_kid(PEDIGREE *p, int par0, int par1)
{
INDIV *ind=NULL;
int i,j;
int mother, father;
int mostleft=0;
double mostleftx;
int generation;
int maxgenerations=0;

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
	(double)(p->indivs[par1]->x);	/* Start on the right of the couple */
  for (i=0; i<p->nbindivs; i++)	/* Update the most left for this couple */
  {
    if (p->indivs[i]->generation == (p->indivs[par0]->generation+1))
    {
      if (is_a_descendant(p,par0, i)>0)
      {
	ind=p->indivs[i];
	if ((double)(p->indivs[i]->x)<(double)(mostleftx))
	{
		mostleftx=(double)(p->indivs[i]->x);  /* i becomes the most left now */
	}
      }
    }
  }
  return (double)mostleftx;
}

/* Same thing but returns the mostrightx position */
double get_couple_mostrightx_kid(PEDIGREE *p, int par0, int par1)
{
INDIV *ind=NULL;
int i,j;
int mother, father;
int mostright=0;
double mostrightx;
int generation;
int maxgenerations=0;

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
        (double)(p->indivs[par0]->x); /* Start on the left of the couple */
  for (i=0; i<p->nbindivs; i++) /* Update the most right for this couple */
  {
    if (p->indivs[i]->generation == (p->indivs[par0]->generation+1))
    {
      if (is_a_descendant(p,par0,i)>0)
      {
        ind=p->indivs[i];
        if ((double)(p->indivs[i]->x)>(double)(mostrightx))
          mostrightx=(double)(p->indivs[i]->x); /* i becomes the most righ pos */
      }  
    }
  }
  return (double)mostrightx;
}


/* The translation occurs for latest generation couple first (!done individuals)
   and translate the kids towards the most left using the minimum possible */
void translate_couple_kids(PEDIGREE *p, int par0, int par1)
{
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

#define MINBOX_SIZE	20.0
#define TOPBORDER	20.0
#define LEFTBORDER	20.0

gdImagePtr draw_indivs(PEDIGREE *p)
{
gdImagePtr im=NULL;
GENERATION *gen=NULL;
int generation;
int i,j,k;
int x=0, y=0;
double xf,yf;
int dx=0; int dy=0;
char strtmp[255]={""};
double ratio=1.0;
double multx=1.0;
int maxx=0;
int maxy=0;
double xfe=0.0;
double dY=0.0;
int *done=NULL;

int mostleft;
double mostleftx;
INDIV *mostleftsib=NULL;
INDIV *mostleftspouse=NULL;
INDIV *lastleftind=NULL;
double nextposx;
int par0,par1;	/* par0 is always most left */
double par0x;
double par1x;
  
  chdir("/tmp");
  done=(int *)malloc(p->nbindivs*sizeof(int));  /* Set the pedigree->nbindivs */
  for (i=0;i<p->nbindivs;i++)
        done[i]=0;                              /* Mark them not analysed */
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
	(double)((double)4.0*(double)RADIUS_CIRCLE*(double)(p->nbgenerations)));

   dY= (double)maxy/(double)(p->nbgenerations+1); /* What is the current dY ratio */

/*******************************************************************************/
/* -> COMPRESSION OF EXTRA BLANK SPACE					       */
/* WE NEED TO COMPUTE THE XPIC POSITION IN THE IMAGE FROM THE X RATIOS         */
/*******************************************************************************/

  /* THE FIRST THING WE DO IS TO SET INITIAL XPIC VALUES FROM X VALUES */

  /* LETS UPDATE THE XPIC FOR !DONE INDIVIDUALS (YPIC=(int)y) */
  for (i=0; i<p->nbindivs; i++)
  {
	p->indivs[i]->xpic=(int)(p->indivs[i]->x);
	p->indivs[i]->ypic=0;	/* WHEN DONE, THIS VALUE IS >0 */
	done[i]=0;
  }
  /* INDIVIDUALS ARE PROCESSED FROM THE LAST GENERATION TO THE FIRST */
  for (generation=p->nbgenerations-1; generation>=1; generation--)
  {
	/* WE NEED VARIABLES TO STORE THE LAST MOSTLEFT INDIVIDUAL */
	/* FIND THE NEXT, !done,  MOST LEFT INDIV IN THE GENERATION */
	/* AND GIVE HIM XPIC=FATHER(X) */

	/* ANY INDIVIDUALS IN ABOVE GENERATIONS CAN NOT PROVIDE HIS XPIC YET */
	for (i=0; i<p->nbindivs; i++)
	{
	  if (p->indivs[i]->generation==generation) /* In CURRENT generation */
	  {  /* INDIV MUST NOT BE DONE ALREADY */
	     if (!done[i])	/* Individual is not locked */
	     {	/* THIS ONE IS MOST LEFT THAN THE CURRENT MOSTLEFT */
		if ((double)(p->indivs[i]->xpic) < (double)(mostleftx))
		{
		   mostleftx=(double)(p->indivs[i]->xpic);	/* THIS IS THE REAL VAL*/
		   mostleft=i; /* IT WILL HAVE ITS MOSTLEFT PARENT->X POSITION */
		}	       /* UNLESS IT IS SINGLE CHILD (nbkids=1) */
	     }
	  }
	}
	/* WE NEED TO GET THE PARENTS OF THIS INDIVIDUAL (OR ITS SPOUSE'S) */
	if (p->indivs[mostleft]->mother>=0)	/* IT IS A SPOUSE -> GET OTHER */
 	   par0=p->indivs[mostleft]->mother;
	else	/* THIS IS WHEN THE MOSTLEFT INDIVIDUAL DOESN'T HAVE PARENTS */
	   par0=p->indivs[p->indivs[mostleft]->spouse]->mother;
	par1=p->indivs[par0]->spouse;	/* Its mother's spouse */

	if ((double)(p->indivs[par0]->xpic)>(double)(p->indivs[par1]->xpic))
	{	/* Invert par0 and par1 */
	        j=par0; par0=par1; par1=j; /* FATHER IS THE MOST LEFT PARENT */
	}
		/* WE GOT THE MOST LEFT NEXT SIBLING TO DEAL WITH */
		/* FOR THIS ROUND OF SIBLINGS TRANSLATION */
	
	/* NOW WE LOCK THIS NEW FOUND MOSTLEFT */
	/* Test if this guy has any siblings at all */
	mostleftsib=p->indivs[mostleft];
	if (mostleftsib->sib<0)	/* ONLY ONE CHILD ! */
	{
	  if (mostleftsib->spouse<0)		/* NOT MARRIED -> exception */
	  {					/* The right parent translates*/
	    p->indivs[par0]->xpic= (int)(p->indivs[par0]->x);
            p->indivs[par1]->xpic= (int)((double)(p->indivs[par0]->xpic)
				        +(double)4.0*(double)RADIUS_CIRCLE);
	    mostleftsib->xpic=(int)(((double)(p->indivs[par0]->xpic)
	    		    +(double)(p->indivs[par1]->xpic)))/
			   (double)2.0;
	    done[mostleft]=1;
	    lastleftind=mostleftsib;
          }
	  else					/* ONE COUPLE -> exception */
	  {					/* indivs = parents */
	    mostleftsib->xpic=(double)(p->indivs[par0]->xpic);
	    nextposx=(double)(mostleftsib->xpic)+(double)4.0*(double)RADIUS_CIRCLE;
	    done[mostleft]=1;
	    mostleftspouse=p->indivs[mostleftsib->spouse];
	    mostleftspouse->xpic=(double)(p->indivs[par1]->xpic);
	    done[mostleftsib->spouse]=1;
	    lastleftind=p->indivs[mostleftsib->spouse];
	  }
	}
	else			/* GOOD ! SOME OTHER SIBLINGS (regular scheme) */
	{		
	  mostleftsib->xpic=(double)(p->indivs[par0]->xpic); /* First sib => par0->x */
	  nextposx=(double)(mostleftsib->xpic)+(double)4.0*(double)RADIUS_CIRCLE;
	  done[mostleft]=1;
	  while ((mostleftsib->spouse>=0)||	/* WHILE STILL POSSIBLE, */
		 (mostleftsib->sib>=0))		/* GET NEXT SIB OR SPOUSE */
	  {
		if (mostleftsib->spouse<=0)	/* SINGLE NOTLAST SIBLING */
		{
		   mostleft=mostleftsib->sib;
		   mostleftsib=p->indivs[mostleft];
	  	   mostleftsib->xpic=nextposx; /* First sib => par0->xpic */
		   mostleftsib->y= (double)TOPBORDER
			          +(double)(((double)generation+1.0)*(double)dY);
	  	   nextposx= (double)(mostleftsib->xpic)
			   +(double)4.0*(double)RADIUS_CIRCLE;
		   done[mostleft]=1;
		   lastleftind=mostleftsib;
		}
		else	/* HERE WE JUST CENTER THE PARENTS A LAST TIME */
		{	/* OVER THEIR KIDS */
		   done[par0]=1;
	  	   p->indivs[par1]->xpic=(double)
		   		get_couple_mostrightx_kid(p, par0, par1);
	  	   nextposx= (double)(p->indivs[par1]->xpic)
			   +(double)4.0*(double)RADIUS_CIRCLE;
		   done[par1]=1;
		}
	  }
          /* NOW WE PUT THE MOST RIGHT PARENT TO THE LAST SIBLING POSITION */
        }
/*HERE*/
  }
  done[0]=1;
  done[1]=1;

#if 1

  if ((p->indivs[0]->xpic)<(p->indivs[1]->xpic))
  	maxx=(int)((double)p->indivs[1]->xpic+(double)LEFTBORDER);
  im = gdImageCreate(maxx+(int)LEFTBORDER,maxy+(int)TOPBORDER);
  ini_colors(im);
	/* DRAW THE LINES FOR COUPLES */
  for (i=0; i<p->nbindivs; i++)
  {
    if (!done[i])
    {
	if (p->indivs[i]->spouse>=0)
	{
	   par0=i; par1=p->indivs[i]->spouse;
	   gdImageLine(im,p->indivs[par0]->xpic, p->indivs[par0]->ypic, p->indivs[par1]->xpic, p->indivs[par1]->ypic,black);
	   gdImageLine(im, (int)((double)(p->indivs[par0]->xpic+p->indivs[par1]->xpic)/(double)2.0), p->indivs[par0]->ypic, (int)((double)(p->indivs[par0]->xpic+p->indivs[par1]->xpic)/(double)2.0), (int)((double)(p->indivs[i]->generation+1)*dY)+(int)((double)dY/(double)2.0), black);
	}
    }
        
  }

	/* DRAW THE INDIVIDUALS */
  for (i=0; i<p->nbindivs; i++)
  {
	x=(int)(p->indivs[i]->xpic);
	y=(int)(p->indivs[i]->ypic);
	if ((p->indivs[i]->father>=0)&&
	    (p->indivs[i]->mother>=0))
	{
		   gdImageLine(im, x, y-RADIUS_CIRCLE,
				   x, (int)((double)(p->indivs[i]->generation)*dY)+(int)((double)dY/(double)2.0), black);
		   gdImageLine(im, x, (int)((double)(p->indivs[i]->generation)*dY)+(int)((double)dY/(double)2.0),
				   (p->indivs[p->indivs[i]->mother]->xpic+p->indivs[p->indivs[i]->father]->xpic)/2,
		   		   (int)((double)(p->indivs[i]->generation)*dY)+(int)((double)dY/(double)2.0),
				   black);
	}
	switch (p->indivs[i]->sex)
        {
            case 0:
                draw_circle(im, x, y, RADIUS_CIRCLE, 1, red);
                break;
            case 1:
                dx=SIDE_SQUARE/2;
                gdImageFilledRectangle(im, x-dx, y-dx, x+dx, y+dx, blue);
                break;
        }
	sprintf(strtmp,"%d",i+1);
        gdImageString(im, gdFontTiny, x-2, y+RADIUS_CIRCLE+4, strtmp, black);
  }
#endif
  return (gdImagePtr)im;
  
}

