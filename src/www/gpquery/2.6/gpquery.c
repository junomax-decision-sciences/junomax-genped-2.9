/* This program does only one thing; add an annotation to the database */

#define VERBOSE 0

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "util.h"
#include "gpconfig.h"
#include "gphtml.h"

#include "ped.h"

#define GP_NO_DIR_ERROR			1
#define GP_LOAD_PEDIGREE_ERROR		2
#define GP_CAN_NOT_CREATE_PEDFILE	3
#define GP_NO_GIF_ERROR			4

/* This is the hard coded maximum for GP_MAX_GENERATIONS */
#define DEF_MAX_GENERATIONS	5

#if RELATION_METHOD1
char relation[DEF_MAX_GENERATIONS][255]= {
	"My spouse and I are the oldest couple in my family.",
	"Individual 1 represents my mother (or my spouse's)",
	"Individual 1 represents my grandmother (or my spouse's)",
	"Individual 1 represents my great-grandmother (or my spouse's)",
	"Individual 1 represents my great-great-grandmother (or my spouse's)"
};
#else
char relation[DEF_MAX_GENERATIONS][255]= {
        "My spouse and I are the oldest couple in my family.",
        "My parents (or my spouse's) are the oldest couple in my family",
        "My grandparents (or my spouse's) are the oldest couple in my family",
        "My great-grandparents (or my spouse's) are the oldest couple in my family",
        "My great-great-grandparents (or my spouse's) are the oldest couple in my family"
};
#endif
char relation2[DEF_MAX_GENERATIONS][255]= {
        "You and your spouse are represented at the top of this pedigree",
        "Your parents (or your spouse's) are represented at the top of this pedigree",
        "Your grandparents (or your spouse's) are represented at the top of this pedigree",
        "Your great-grandparents (or your spouse's) are represented at the top of this pedigree",
        "Your great-great-grandparents (or your spouse's) are represented at the top of this pedigree",
};

	

static output_gif_file( char *tmpgif)
{
int fdd=-1;
long n=0;
char buf[1024]={""};
  if ((fdd=open(tmpgif,O_RDONLY))<0)
  {
      print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Can't open graphic alignment");
  }
  setbuf(stdout,0);
  printf("Content-Type: image/gif%c%c",10,10);
  while ((n=read(fdd,buf,1024))>0)
      write(1,buf,n);
  close(fdd);
}

typedef struct {
    char name[8096];
    char val[8096];
} entry;
entry entries[255];

static char save_answer[16000]={""};

#define HTML_TAB	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"

#define MAX_OPTIONS	8
static char pairoptions[MAX_OPTIONS+1][255]={
	"MAIN_PAGE",
	"ACTIVETOP",
	"ACTIVEBOT",
	"BUILDPEDIGREE",
	"TICKET",
	"SHOW_PEDIGREE",
	"SHOW_PED_PAGE",
	"NEXT_FORM"
};


print_usage(char *progname)
{
	exit(1);
}


get_pair_option(char *name)
{
 int i=0;
  for (i=0;i<MAX_OPTIONS; i++)
  {
	if (strncmp(name,pairoptions[i],strlen(pairoptions[i]))==0)
		return (int)i;
  }
  return -1;
}

#define MAX_KIDS	7

print_pheno_row(PEDIGREE *p, char *Ticket)
{
int i=0;
  make_sub_cell("white","white",DB_FONT_COLOR);
  printf("%s<FONT COLOR=black>",TIMES_10);
  printf("<TABLE BORDER=0 WIDTH=100%%><FORM METHOD=POST ACTION=%s><INPUT TYPE=HIDDEN NAME=PROCESS_PHENO VALUE=%s<TR>",DB_NAME,Ticket);
  for (i=0; i<p->nbindivs; i++)
  {
	if ((i!=0)&&(i%10==0))
		printf("</TR><TR>");
	printf("<TD BGCOLOR=white ALIGN=RIGHT>%d <INPUT TYPE=CHECKBOX NAME=PHENO VALUE=%d></TD>",i+1,i+1);
  }
  if (p->nbindivs%10!=0)
  {
    for (i=p->nbindivs; i<10-p->nbindivs%10; i++)
	printf("<TD BGCOLOR=white>&nbsp;</TD>");
  }
  printf("</TR></TABLE></FONT></FONT>");
  close_cell();
  printf("<CENTER><A HREF=\"javascript:back()\"><IMG BORDER=0 SRC=%s/gifs/previouspage.jpg ALT=\"Click to go back to the previous page...\"></A>",GP_HTDOCS_DIR);
  printf(" <INPUT TYPE=IMAGE BORDER=0 SRC=%s/gifs/nextpage.jpg ALT=\"Click to proceed...\" onClick=\"document.forms[0].submit()\">",GP_HTDOCS_DIR);
  printf("</FORM>");
}

print_couple_row(int mother, int father)
{
int i=0;
  printf("<TD BGCOLOR=white>");
  printf("<CENTER>%s<FONT COLOR=black><B>Couple(%d,%d)</B></FONT></FONT>%s<FONT COLOR=black> has </FONT></FONT>\n",TIMES_10,mother,father,TIMES_8);
  printf("</TD>");
  printf("<INPUT TYPE=HIDDEN NAME=COUPLE VALUE=%d_%d>\n",mother,father);
  printf("<TH BGCOLOR=white>%s\n",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=C_D>\n");
  for (i=0; i<=MAX_KIDS; i++)
	printf("\t<OPTION VALUE=%d>%d</OPTION>\n", i,i);
  printf("</SELECT>\n");
  printf("</TH>");
  printf("<TH BGCOLOR=white>%s\n",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=S_D>\n");
  for (i=0; i<=MAX_KIDS; i++)
  {
    if (i!=1)
	printf("\t<OPTION VALUE=%d>%d</OPTION>\n", i,i);
    else
	printf("\t<OPTION VALUE=%d SELECTED>%d</OPTION>\n", i,i);
  }
  printf("</SELECT>");
  printf("</TH>");
  printf("<TH BGCOLOR=white>%s\n",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=C_S>\n");
  for (i=0; i<=MAX_KIDS; i++)
	printf("\t<OPTION VALUE=%d>%d</OPTION>\n", i,i);
  printf("</SELECT>\n");
  printf("</TH>\n");
  printf("<TH BGCOLOR=white>%s\n",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=S_S>\n");
  for (i=0; i<=MAX_KIDS; i++)
	printf("\t<OPTION VALUE=%d>%d</OPTION>\n", i,i);
  printf("</SELECT>\n");
  printf("</TH>\n");
}

print_couple_row2(int mother, int father)
{
int i=0;
  printf("<CENTER><TABLE BORDER=0 WIDTH=100%%><TR>");
  printf("<INPUT TYPE=HIDDEN NAME=COUPLE VALUE=%d_%d>",mother,father);
  printf("<TD BGCOLOR=white>%s",TIMES_10);
  printf("Couple(%d,%d) has ",mother,father);
  printf("</TD><TD BGCOLOR=white>%s",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=C_D>");
  for (i=0; i<=MAX_KIDS; i++)
        printf("<OPTION VALUE=%d>%d</OPTION>", i,i);
  printf("</SELECT> daughters with child(ren), ");
  printf("</TD><TD BGCOLOR=white>%s",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=S_D>");
  for (i=0; i<=MAX_KIDS; i++)
  {
    if (i!=1)
        printf("<OPTION VALUE=%d>%d</OPTION>", i,i);
    else
        printf("<OPTION VALUE=%d SELECTED>%d</OPTION>", i,i);
  }
  printf("</SELECT> daughters without child(ren), ");
  printf("</TD></TR>");
  printf("<TR><TD BGCOLOR=white>&nbsp;</TD>");
  printf("<TD BGCOLOR=white> %s",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=C_S>");
  for (i=0; i<=MAX_KIDS; i++)
        printf("\t<OPTION VALUE=%d>%d</OPTION>", i,i);
  printf("</SELECT> sons with child(ren) and ");
  printf("</TD><TD BGCOLOR=white>%s",TIMES_10);
  printf("<SELECT TYPE=MENU NAME=S_S>");
  for (i=0; i<=MAX_KIDS; i++)
        printf("\t<OPTION VALUE=%d>%d</OPTION>", i,i);
  printf("</SELECT> sons without child(ren).");
  printf("</TD></TR></TABLE></CENTER><BR>");
}

print_error_page(char *background, char *foreground, char *linkground, char *font, char *title)
{
	printf("Content-Type: text/html\n\r\n\r");
	printf("<HTML><HEAD><TITLE>%s</TITLE></HEAD>",title);
        printf("<BODY BGCOLOR=%s TEXT=%s LINK=%s VLINK=%s ALINK=%s>",
                background, foreground, linkground,linkground,linkground);
        printf("%s<FONT COLOR=%s>",font,foreground);
	printf("%s",title);
        printf("</FONT></FONT>");
        printf("</BODY></HTML>");
	exit(0);
}

#define MAX_QUERYSIZE	64000

int main(int argc, char **argv)
{
int relat=-1;
PEDIGREE *ped2=NULL;
int pixels=0;
char cache=0;
char kidfound=0;
time_t Sometime2;
int lastsib=0;
int sibcount=0;
int nbkids=0;
int pos=0; 
int pos2=0;
char dirname[255]={""};
int mother=-1;
int father=-1;
char pedfile[255]={""};
char title[255]={""};
FILE *fd=NULL;
char already=0;
unsigned char cc=0;
long j=0;
INDIV *leftind=NULL;
INDIV *rightind=NULL;
int cl=0;
long i=0;
int generation=0;
long n=0;
char buf[1024]={""};
int fdd=0;
int fdi=0;
time_t Sometime=0;
char strtmp[255]={""};
char strtmp2[255]={""};
char *c2;
int mm=0;
char ispost=1;
long ticket=0;
char Ticket[255]={""};
register int x,m=0;
char method=0;	/* POST if 1 then GET */
char done=0;
int *dones;
char found=0;
char line[8096]={""};
char dbn[255]={""};
char *c=NULL;
int count=0;
int nb_option=0;
pid_t pid;
PEDIGREE *ped=NULL;
COUPLE *couple=NULL;
INDIV *ind=NULL;

  chdir("/tmp");
  strcpy(dbn,argv[0]);
  gp_get_config(dbn); 
  if (strcmp(DB_NAME,dbn)!=0)
  i=0;
  c2=getenv("REQUEST_METHOD");
        if(strncmp(c2,"GET",3)==0)
        {
                c = getenv("QUERY_STRING");
                if(c == NULL) {
                  exit(1);
                }
                for(x=0;c[0] != '\0';x++) {
                  m=x;
                  getword(entries[x].val,c,'&');
                  plustospace(entries[x].val);
                  unescape_url(entries[x].val);
                  getword(entries[x].name,entries[x].val,'=');
                  count++;
                }
        }
        else
        if(strncmp(c2,"POST",4)==0)
        {
              c=getenv("CONTENT_LENGTH");
              cl = atoi(c);
              for(x=0;cl && (!feof(stdin));x++)
              {
                m=x;
                strcpy(entries[x].val,(char *)fmakeword(stdin,'&',&cl));
                plustospace(entries[x].val);
                unescape_url(entries[x].val);
                strcpy(entries[x].name,(char *)makeword(entries[x].val,'='));
                count++;
              }
        }
        else
        {
		print_cleartop(1,0,1,"GenPed Error","white","black");
                printf("<FONT COLOR=black>PROBLEM WITH POST[%s]/GET[%s] TRANSMIS
SION<br>\n",c,c2);
        }

        if (count==0)
  	{
	  printf("Content-Type: text/html\n\r\n\r");
	  printf("<HTML><HEAD><TITLE>%s - Copyright(c)1999 GenPed Inc.</TITLE>",DB_HTML_NAME);
          printf("</HEAD>");
          printf("<FRAMESET ROWS=\"50,100%%,40\" BORDER=0>\n");
	  printf("<FRAME name=ACTIVETOP src=%s?ACTIVETOP=1 noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",DB_NAME);
          printf("<FRAMESET ROWS=\"0,100%%,0\" BORDER=0>\n");
	  printf("<FRAME name=LEFTFRAME src=%s/noframe.html noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",GP_HTDOCS_DIR);
	  printf("<FRAME name=MAINFRAME src=%s?MAIN_PAGE=1 noresize scrolling=auto marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",DB_NAME);
	  printf("<FRAME name=RIGHTFRAME src=%s/noframe.html noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",GP_HTDOCS_DIR);
          printf("</FRAMESET>\n");
	  printf("<FRAME name=ACTIVEBOT src=%s?ACTIVEBOT=1 noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",DB_NAME);
          printf("</FRAMESET>\n");
          printf("</HTML>");
          exit(0);
  	}
	nb_option=get_pair_option(entries[0].name);
  	switch(nb_option)
  	{
    	  case 0:
		print_cleartop(1,0,1,"Building a pedigree","black","white");
		printf("<BR><BR><BR><CENTER>");
		
		printf("%s<A HREF=%s?WHATISPEDIGREE=1 TARGET=MAINFRAME><FONT COLOR=%s>What is a Pedigree ?</FONT></A></FONT>",TIMES_14,DB_NAME,DB_FONT_COLOR);
		printf("<P>");
		Sometime=time(&Sometime);
		printf("%s<A HREF=\"javascript://\" onClick=\"window.open('%s?BUILDPEDIGREE=%ld','MAINPAGE','scrollbars=yes,resizable=yes,width=800,height=600,top=10,left=10');\"><FONT COLOR=%s>Build Your Own Family Pedigree</FONT></A></FONT>",TIMES_14,DB_NAME,Sometime,DB_FONT_COLOR);
		printf("</CENTER>");
		exit(0);
		break;
    	case 1:    /* ACTIVETOP */
		print_cleartop(1,0,1,"Building a pedigree","black","white");
		make_sub_cell("white","white",DB_FONT_COLOR);
	        printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
		printf("<TH BGCOLOR=WHITE WIDTH=100%%>%s<FONT COLOR=%s>%s</FONT></FONT></TH>",TIMES_14, DB_FONT_COLOR,DB_HTML_NAME);
		printf("</TR></TABLE>");
		close_cell();
		printf("</BODY></HTML>");
		break;
    	case 2:    /* ACTIVEBOT */
		print_cleartop(1,0,1,"Building a pedigree","black","white");
		make_sub_cell("white","white",DB_FONT_COLOR);
		printf("<CENTER>%s<FONT COLOR=%s><I>Powered by <A HREF=http://www.genped.com>GenPed</A>. Copyright (c)1999 </I></FONT></FONT></CENTER></BODY></HTML>",TIMES_12,DB_FONT_COLOR);
		close_cell();
		exit(0);
		break;
    	case 3:	/* BUILD PEDIGREE */
		sprintf(title,"Starting your family new pedigree...");
		
		Sometime=time(&Sometime);
		sprintf(strtmp,"%s - %s",ctime(&Sometime));
		strtmp[strlen(strtmp)-1]='\0';
		print_cleartop(1,0,1,strtmp,"black","white");
		printf("<CENTER><BR>%s<FONT COLOR=black><U><B>How To Build Your Family Pedigree ?</B></U></FONT></FONT></CENTER>",TIMES_12);
		pid=getpid();
		sprintf(Ticket,"%d-%ld",getpid(),Sometime);
		printf("<FORM METHOD=GET ACTION=%s>", DB_NAME);
		printf("<INPUT TYPE=HIDDEN NAME=TICKET VALUE=%s>", Ticket);
		printf("<INPUT TYPE=HIDDEN NAME=GENERATION VALUE=0>");
 		cache=0;
#if 0
		printf("<CENTER><IMG SRC=%s/gifs/gpsep500.jpg><P><TABLE BORDER=0 WIDTH=100%%><TR>",GP_HTDOCS_DIR);
#endif
		printf("<CENTER><TABLE BORDER=0 WIDTH=100%%><TR>");
		printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		printf("<TD BGCOLOR=white>");
		printf("%s<FONT COLOR=black><P ALIGN=JUSTIFY>",TIMES_10);
		printf("%sBefore adding individuals to your pedigree, you need to identify the oldest living couple in your family <B>where %s can be traced</B>. For example, if you or your spouse are the oldest individuals in your family, you would both constitute this couple.\n</P></FONT></FONT>", HTML_TAB,DB_DISORDER_NAME);
		printf("</TD><TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		printf("</TR></TABLE>");
/***** IF GP_MAX_GENERATIONS>0 *****/
		if (GP_MAX_GENERATIONS!=0)
		{
		   printf("<CENTER><P><TABLE BORDER=0 WIDTH=100%%><TR>",GP_HTDOCS_DIR);
		   printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		   printf("<TD BGCOLOR=white>%s<FONT COLOR=black><P ALIGN=LEFT>%s<U>Please indicate below your family relationship to this couple:</U>\n</P>", TIMES_10, HTML_TAB);
		   printf("<CENTER>%s<SELECT TYPE=MENU NAME=RELATION>",HTML_TAB);
		/* This represents the maximum generations */
		  for (i=0; i<GP_MAX_GENERATIONS; i++)
		  {
		   printf("<OPTION VALUE=%d>%s</OPTION>",i,relation[i]);
		  }
		  printf("</SELECT>\n</CENTER>\n");
		  printf("</FONT></FONT>");
		  printf("</TD>");
		  printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		  printf("</TH>");
		  printf("</TR>");
		  printf("</TABLE><BR>");
		}
/***** PRINT COUPLE0 ROWS OF CHILDREN *****/
		printf("<CENTER><IMG SRC=%s/gifs/oldestcouple.jpg></CENTER>",GP_HTDOCS_DIR);
		printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
		printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		printf("<TD BGCOLOR=white>");
		printf("%s<FONT COLOR=black><P ALIGN=JUSTIFY>",TIMES_10);
		printf("%sNext, you need to indicate in the form below how many child(ren) this first couple has, then go to the next page...\n</P>",HTML_TAB);
		printf("</TD><TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		printf("</TR></TABLE><CENTER>");
		printf("<P><TABLE BORDER=0 WIDTH=100%%><TR>");
		printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
		printf("<TD BGCOLOR=white WIDTH=10%%>&nbsp;</TD><TD BGCOLOR=white>");
		printf("%s<FONT COLOR=black><P ALIGN=JUSTIFY>",TIMES_10);
		print_couple_row2(1,2);
		printf("\n</P></FONT></FONT>\n");
		printf("</TD><TD BGCOLOR=white WIDTH=10%%>&nbsp;</TD>");
		printf("</TR></TABLE></CENTER><BR>");
#if 0
		  printf("<CENTER><IMG SRC=%s/gifs/gpsep500.jpg><BR>",GP_HTDOCS_DIR);
#endif
		printf("<CENTER><BR>");
		printf("<INPUT TYPE=IMAGE BORDER=0 SRC=%s/gifs/nextpage.jpg ALT=\"Click to proceed to the next generation...\" onClick=\"document.forms[0].submit()\">",GP_HTDOCS_DIR);
		printf("</FORM>");
		printf("</BODY></HTML>");
		exit(0);
		break;
	case 4: /* COMPLETE PEDIGREE */
		Sometime=time(&Sometime);
		if (strcpy(Ticket,entries[0].val)==NULL)
		   print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Invalid Query");
		if (strcmp(entries[1].name,"GENERATION")!=0)
		   print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Invalid Query");
		if (sscanf(entries[1].val,"%d",&generation)!=1)
		   print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Invalid Form (Generation)");
		if (strcmp(entries[2].name,"RELATION")==0)
		{
		  if (sscanf(entries[2].val,"%d",&relat)!=1)
			relat=-1;	/* Ignore relation if not valid */
		}
		sprintf(dirname,"%s/data/%s/spool/%s", GP_ROOT_DIR, DB_NAME, Ticket);
		if (generation==0)	/* MAKE THE FIRST PEDIGREE FILE */
		{
		  umask(0113);
                  mkdir(dirname,02775);
		  chmod(dirname,02775);
		  if (chdir(dirname)<0)
		  {
		    sprintf(strtmp,"GenPed Error Code %d (%s)", GP_NO_DIR_ERROR,dirname);
		    print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, strtmp);
		  }
		  sprintf(pedfile,"%s/ped.%d", dirname, generation);
		  if ((fd=fopen(pedfile,"w"))!=NULL) /* Create initial ped.0 */
		  {
	              fprintf(fd,"1 0 0 0 2 0 0 0 0 0\n");
	              fprintf(fd,"2 1 0 0 1 0 0 0 0 0\n");
		  }
		  else
		  {
		    sprintf(strtmp,"GenPed Error Code %d", GP_CAN_NOT_CREATE_PEDFILE);
		    print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, strtmp);
		  }
		  fclose(fd);
		}
		else	/* Loads previous pedigree individuals */
		  sprintf(pedfile,"%s/ped.%d", dirname, generation);
		if ((ped=load_pedigree(pedfile,1))==NULL)	/* Only Load */
		{
		    sprintf(strtmp,"GenPed Error Code %d (%s)", GP_LOAD_PEDIGREE_ERROR,pedfile);
		    print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, strtmp);
		}
		Sometime=time(&Sometime);
		sprintf(title,"%s - %s", strtmp, ctime(&Sometime));
		title[strlen(title)-1]='\0';
		cache=0;
		while (i<count)
		{
		   if (strcmp(entries[i].name,"CACHE")==0)
			cache=1;
		   if (strcmp(entries[i].name,"SCALE")==0)
		   {
			i++;
			if (sscanf(entries[i].val,"%d",&pixels)!=1)
				pixels=0;
		   }
		   i++;
		}
#if VERBOSE
		printf("Your ticket number is [%s]<BR>",Ticket);
		printf("%d Individuals in Pedigree so far.<BR>",ped->nbindivs);
#endif
		generation=generation+1;
		i=2;
		j=ped->nbindivs+1;
		mother=-1;
		father=-1;
		found=0;
		for (i=0; i<ped->nbindivs; i++)
		{
		  ind=ped->indivs[i];
#if VERBOSE
		  printf("%d %d %d %d %d %d %d %d %d<BR>",
			i+1, ind->sex, ind->mother+1, ind->father+1,
			ind->spouse+1, ind->kid+1,
			ind->sib+1, ind->proband,
                	ind->pheno);
#endif
		}
		couple=(COUPLE *)create_couple();
		i=1;
		while (i<count)
		{
			/* Get another couple */
		  if (strcmp(entries[i].name,"COUPLE")==0)
		  {
			if (strcpy(strtmp,entries[i].val)!=NULL)
			{
			   for (j=0; j<strlen(strtmp); j++)
				if (strtmp[j]=='_')
					strtmp[j]=' ';
			   sscanf(strtmp,"%d %d", &mother, &father);
			}
			couple->mother_id=mother-1;
			couple->father_id=father-1;
		  	i++;
			sscanf(entries[i].val,"%d",&(couple->nb_cdaughters));
			i++;
			sscanf(entries[i].val,"%d",&(couple->nb_sdaughters));
			i++;
			sscanf(entries[i].val,"%d",&(couple->nb_csons));
			i++;
			sscanf(entries[i].val,"%d",&(couple->nb_ssons));
			i++;
#if VERBOSE
			printf("COUPLE %d-%d: %d %d %d %d<BR>",
				couple->mother_id, couple->father_id,
				couple->nb_cdaughters, couple->nb_sdaughters,
				couple->nb_csons, couple->nb_ssons);
#endif
			nbkids=couple->nb_cdaughters+couple->nb_sdaughters+
				couple->nb_csons+couple->nb_ssons;
			if (nbkids==0)
			{
#if 1
			  print_cleartop(1,0,0,"Error Couple Submission","black","white");
#endif
			  printf("<CENTER><TABLE BORDER=0 WIDTH=100%%><TR>");
			  printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
			  printf("<TD BGCOLOR=white>");
			  make_sub_cell("white","white",DB_FONT_COLOR);
			  printf("%s<FONT COLOR=black>Couple (%d,%d) does not have any children ! Please push the Back Button and make sure each couple has at least one child.</FONT></FONT>",TIMES_12, couple->mother_id+1, couple->father_id+1);
			  printf("</FONT></FONT>");
			  close_cell();
			  printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
			  printf("</TH>");
			  printf("</TR></TABLE>");
  			  free(couple);
        		  free_pedigree(ped);
		          exit(0);
			}
			else	/* Ok we have some kids to add */
			{	/* FOR LAST SIBLING MUST BE LAST IN GENERATION */
			  sibcount=0;
			  kidfound=0;
			  for (j=0; j<couple->nb_cdaughters;j++)
			  {
			    found=1;
			    ped->indivs=(INDIV **)realloc((INDIV **)(ped->indivs),
				      (ped->nbindivs+sibcount+2)*sizeof(INDIV *));
			    ped->indivs[ped->nbindivs+sibcount]=(INDIV *)
						create_indiv(); /*CDAUGHTER*/
			    ind=ped->indivs[ped->nbindivs+sibcount];
			    if (!kidfound)
			    {
				ped->indivs[couple->mother_id]->kid=ped->nbindivs+sibcount;
				ped->indivs[couple->father_id]->kid=ped->nbindivs+sibcount;
				kidfound=1;
			    }
			    ind->id= ped->nbindivs+sibcount;
			    ind->generation=generation;
			    ind->sex=0;	/* wife (CHILD) */
			    ind->mother=couple->mother_id;
			    ind->father=couple->father_id;
			    ind->spouse=ped->nbindivs+sibcount+1;
			    ind->kid=-1;
			    ped->indivs[ped->nbindivs+sibcount+1]=(INDIV *)
						create_indiv(); /*SPOUSE*/
			    ind=ped->indivs[ped->nbindivs+sibcount+1];
			    ind->id= ped->nbindivs +sibcount +1;
			    ind->sex=1;	/* husband */
			    ind->mother=-1;
			    ind->father=-1;
			    ind->spouse=ped->nbindivs+sibcount;
			    ind->kid=-1; ind->sib=-1;
			    sibcount=sibcount+2;
			  }
			  for (j=0; j<couple->nb_csons; j++)
                          {
			    found=1;
                            ped->indivs=(INDIV **)realloc((INDIV **)(ped->indivs),
                                      (ped->nbindivs+sibcount+2)*sizeof(INDIV *));
                            ped->indivs[ped->nbindivs+sibcount]=(INDIV *)
                                                create_indiv(); /*CDAUGHTER*/
                            ind=ped->indivs[ped->nbindivs+sibcount];
			    if (!kidfound)
			    {
				ped->indivs[couple->mother_id]->kid=ped->nbindivs+sibcount;
				ped->indivs[couple->father_id]->kid=ped->nbindivs+sibcount;
				kidfound=1;
			    }
                            ind->id= ped->nbindivs +sibcount ;
                            ind->generation=generation;
                            ind->sex=1; /* husband (CHILD) */
                            ind->mother=couple->mother_id;
                            ind->father=couple->father_id;
                            ind->spouse=ped->nbindivs+sibcount+1;
                            ind->kid=-1;
			    ind->sib=-1;
                            ped->indivs[ped->nbindivs+sibcount+1]=(INDIV *)
                                                create_indiv(); /*SPOUSE*/
			    ind=ped->indivs[ped->nbindivs+sibcount+1];
			    ind->id=ped->nbindivs+sibcount+1;
                            ind->generation=generation;
                            ind->sex=0; /* girl */
                            ind->mother=-1;
                            ind->father=-1;
                            ind->spouse=ped->nbindivs+sibcount;
                            ind->kid=-1;
			    ind->sib=-1;
                            sibcount=sibcount+2;
                          }
			  for (j=0; j<couple->nb_sdaughters; j++)
                          {
                            ped->indivs=(INDIV **)realloc((INDIV **)(ped->indivs),
                                      (ped->nbindivs+sibcount+1)*sizeof(INDIV *));
                            ped->indivs[ped->nbindivs+sibcount]=(INDIV *)create_indiv(); /*CDAUGHTER*/
			    if (!kidfound)
                            {   
                                ped->indivs[couple->mother_id]->kid=
					ped->nbindivs+sibcount;
                                ped->indivs[couple->father_id]->kid=
					ped->nbindivs+sibcount;
                                kidfound=1;
                            }
                            ind=ped->indivs[ped->nbindivs+sibcount];
                            ind->id= ped->nbindivs +sibcount;
                            ind->generation=generation;
                            ind->sex=0; /* girl (CHILD) */
                            ind->mother=couple->mother_id;
                            ind->father=couple->father_id;
                            ind->spouse=-1;
                            ind->kid=-1;
                            sibcount=sibcount+1;
                          }
                          for (j=0; j<couple->nb_ssons;j++)
                          {
                            ped->indivs=(INDIV **)realloc((INDIV **)(ped->indivs),
                                      (ped->nbindivs+sibcount+1)*sizeof(INDIV *));
                            ped->indivs[ped->nbindivs+sibcount]=(INDIV *)create_indiv(); /*CDAUGHTER*/
                            ind=ped->indivs[ped->nbindivs+sibcount];
			    if (!kidfound)
			    {
				ped->indivs[couple->mother_id]->kid=ped->nbindivs+sibcount;
				ped->indivs[couple->father_id]->kid=ped->nbindivs+sibcount;
				kidfound=1;
			    }
                            ind->id= ped->nbindivs +sibcount;
                            ind->generation=generation;
                            ind->sex=1; /* boy (CHILD) */
                            ind->mother=couple->mother_id;
                            ind->father=couple->father_id;
			    ind->spouse=-1;
                            ind->kid=-1;
			    ind->sib=-1;
                            sibcount=sibcount+1;
                          }
				/* SET THE SIBLINGS */
			  lastsib=-1;
			  for (j=ped->nbindivs+sibcount-1;j>=ped->nbindivs;j--)
			  {
			    ind=ped->indivs[j];
			    if ((ind->father>=0)&&
				(ind->mother>=0))
			    {
#if VERBOSE
			printf("FOUND SIBLING %d (%d)<BR>",j,ind->sib);
#endif
				ind->sib=lastsib;
				lastsib=j;
			    }
		   	  }
			  ped->nbindivs=ped->nbindivs+sibcount;
			}
		  }
		  else i++;
		}
#if VERBOSE
		printf("%d INDIVS<BR>",ped->nbindivs);
		for (i=0; i<ped->nbindivs; i++)
		{
		   ind=ped->indivs[i];
		   printf("%d %d %d %d %d %d %d<BR>\n",
			ind->id+1, ind->sex, ind->mother+1, ind->father+1,
			ind->spouse+1, ind->kid+1, ind->sib+1);
		}
#endif
		sprintf(pedfile,"%s/ped.%d", dirname,generation);
		fd=fopen(pedfile,"w");
		for (i=0; i<ped->nbindivs; i++)
		{
		   ind=ped->indivs[i];
		   fprintf(fd,"%d %d %d %d %d %d %d 0 0 0\n",
			ind->id+1, ind->sex, ind->mother+1, ind->father+1,
			ind->spouse+1, ind->kid+1, ind->sib+1);
		}
		fclose(fd);
		free_pedigree(ped);
		if ((ped=(PEDIGREE *)load_pedigree(pedfile,0))==NULL)	/* Only Load */
		{
		    sprintf(strtmp,"GenPed Error Code %d", GP_LOAD_PEDIGREE_ERROR);
		    print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, strtmp);
		}
		Sometime2=0;
		Sometime2=time(&Sometime2);
                sprintf(strtmp,"%s/data/%s/tmp/%ld.gif", GP_ROOT_DIR, DB_NAME, Sometime2);
#if 0
		printf("<HR>%s:%d indivs ->[%s]<HR>",pedfile,ped->nbindivs,strtmp);
#endif
                gif2pedigree(ped, strtmp);
#if 0
                if (strcpy(Ticket,entries[0].val)==NULL)
                        print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Unknown or Invalid Query (6-0)");
                if (strcmp(entries[1].name,"GENERATION")!=0)
                        print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Unknown or Invalid Query (6-1)");
#endif
		cache=0;
		i=1;
                while (i<count)
                {
                   if (strcmp(entries[i].name,"CACHE")==0)
                        cache=1;
                   if (strcmp(entries[i].name,"SCALE")==0)
                   {
                        i++;
                        if (sscanf(entries[i].val,"%d",&pixels)!=1)
                                pixels=0;
                   }
                   i++;
                }
                printf("Content-Type: text/html\n\r\n\r");
                printf("<HTML><HEAD><TITLE>%s - Copyright(c)1999 GenPed Inc.</TITLE>", DB_HTML_NAME);
                printf("</HEAD>");
                printf("<FRAMESET COLS=\"100%%\" BORDER=0>\n");
#if 0
                printf("<FRAME name=LEFTFRAME src=%s/noframe.html noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",GP_HTDOCS_DIR);
#endif
                printf("<FRAMESET ROWS=\"%d,*\">\n",40*(generation+1)+60);
                printf("<FRAME name=PAGETOP src=%s?SHOW_PED_PAGE=%ld scrolling=auto marginwidth=0 marginheight=0 frameborder=0 borderheight=0 borderwidth=0>",DB_NAME, Sometime2);
		if (relat<0)
                  printf("<FRAME name=PAGEBOT src=%s?NEXT_FORM=%s&GENERATION=%d", DB_NAME,Ticket,generation);
		else
                  printf("<FRAME name=PAGEBOT src=%s?NEXT_FORM=%s&GENERATION=%d&RELATION=%d", DB_NAME,Ticket,generation,relat);
		if (pixels<0) pixels=0;
		if (pixels>200) pixels=200;
		if (pixels>0) printf("&SCALE=1&RATIO=%d",pixels);
		if (cache) printf("&CACHE=%d",cache);
		printf(" scrolling=auto marginwidth=0 marginheight=0 frameborder=1 borderheight=0 borderwidth=0>");
		if (relat<0)
                  printf("<FRAME name=PAGEBOT src=%s?NEXT_FORM=%s&GENERATION=%d scrolling=auto marginwidth=0 marginheight=0 frameborder=1 borderheight=0 borderwidth=0>",DB_NAME, Ticket, generation);
		else
                  printf("<FRAME name=PAGEBOT src=%s?NEXT_FORM=%s&GENERATION=%d&RELATION=%d scrolling=auto marginwidth=0 marginheight=0 frameborder=1 borderheight=0 borderwidth=0>",DB_NAME, Ticket, generation,relat);
                printf("</FRAMESET>\n");
                printf("</FRAMESET>\n");
                printf("</HTML>");
  		free_couple(couple);
        	free_pedigree(ped);
                exit(0);
                break;
    	    case 5:	/* SHOW_PEDIGREE */
#if 0
		printf("Content-Type: text/html\r\n\r\n");
		printf("%s=[%s]\n", entries[0].name, entries[0].val);
		exit(0);
#endif
		sprintf(strtmp,"%s/data/%s/tmp/%s.gif", GP_ROOT_DIR, DB_NAME, entries[0].val);
        	if ((fdd=open(strtmp,O_RDONLY))<0)
        	{
		    sprintf(strtmp2,"GenPed Error Code %d (%s)", GP_NO_GIF_ERROR,strtmp);
		    print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, strtmp2);
		}
        	setbuf(stdout,0);
		printf("Content-Type: image/gif%c%c",10,10);
        	while ((n=read(fdd,buf,1024))>0)
                        write(1,buf,n);
        	close(fdd);
        	/*remove(strtmp);*/
        	exit(0);
        	break;
            case 6:     /* SHOW_PED_PAGE PAGETOP */
		cache=0;
		pixels=0;
		i=0;
                while (i<count)
                {
                   if (strcmp(entries[i].name,"CACHE")==0)
                        cache=1;
                   if (strcmp(entries[i].name,"SCALE")==0)
                   {
                        i++;
                        if (sscanf(entries[i].val,"%d",&pixels)!=1)
                                pixels=0;
                   }
                   i++;
                }
		sprintf(title,"Pedigree Page %ld - %s", Sometime,ctime(&Sometime));
                title[strlen(title)-1]='\0';
                print_cleartop(1,0,cache,title,"black","white");
		printf("<CENTER>");
		if (pixels>0)
		  printf("<IMG SRC=%s?SHOW_PEDIGREE=%s WIDTH=%s%%><BR>",DB_NAME,entries[0].val,pixels);
		else
		  printf("<IMG SRC=%s?SHOW_PEDIGREE=%s><BR>",DB_NAME,entries[0].val);
		exit(0);
                break;
	     case 7:	/* NEXT FORM */
#if 1	
                if (strcpy(Ticket,entries[0].val)==NULL)
                   print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Invalid Query");
                if (strcmp(entries[1].name,"GENERATION")!=0)
                   print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Invalid Query");
                if (sscanf(entries[1].val,"%d",&generation)!=1)
                   print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Invalid Form (Generation)");
#endif
		if (strcmp(entries[2].name,"RELATION")==0)
                {
                  if (sscanf(entries[2].val,"%d",&relat)!=1)
                        relat=-1;       /* Ignore relation if not valid */
                }
                sprintf(dirname,"%s/data/%s/spool/%s", GP_ROOT_DIR, DB_NAME, Ticket);
                sprintf(title,"%s - Copyright(c)1996,99 GenPed", DB_HTML_NAME);
                title[strlen(title)-1]='\0';
                print_cleartop(1,0,1,title,"black","white");
		sprintf(pedfile,"%s/ped.%d", dirname, generation);
                if ((ped=load_pedigree(pedfile,0))==NULL)       /* Only Load */
                {
                    sprintf(strtmp,"GenPed Error Code %d (%s)", GP_LOAD_PEDIGREE_ERROR,pedfile);
                    print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, strtmp);
                }
#if VERBOSE
                printf("Your ticket number is [%s]<BR>",Ticket);
                printf("%d Individuals in Pedigree so far.<BR>",ped->nbindivs);
#endif
		found=0;
		for (i=0; (i<ped->nbindivs)&&(!found); i++)
                {
                    ind=ped->indivs[i];
                    if (ind->generation==generation)
                    {
                      if (ind->spouse>=0)
			found=1;
                    }
                }
		if (found)
		{
		  printf("<FORM METHOD=GET ACTION=%s TARGET=MAINPAGE>\n", DB_NAME);
		  printf("<INPUT TYPE=HIDDEN NAME=TICKET VALUE=%s>\n", Ticket);
		  printf("<INPUT TYPE=HIDDEN NAME=GENERATION VALUE=%d>\n",generation);
		  if (relat>=0)
		  	printf("<INPUT TYPE=HIDDEN NAME=RELATION VALUE=%d>\n",relat);
		  printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
		  printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
		  printf("<TD BGCOLOR=white>");
		  if (relat<0)
			  printf("<P ALIGN=JUSTIFY>%s<FONT COLOR=black>You can now add new individuals in the ",TIMES_10);
		  else
			  printf("<P ALIGN=JUSTIFY>%s<FONT COLOR=black>%s. You can now add new individuals in the ",TIMES_10,relation2[relat]);
			  switch(generation%10+3)
				{
				   case 1:
				       printf(" <U>%dst generation</U> ",generation+2);
					break;
				   case 2:
				       printf(" <U>%dnd generation</U> ",generation+2);
					break;
				   case 3:
				       printf(" <U>%drd generation</U> ",generation+2);
					break;
				   default:
				       printf(" <U>%dth generation</U> ",generation+2);
					break;
				}
			  printf("of your family pedigree above. Please indicate how many children each couple listed below has:\n");
		  if (generation==relat-1)
		  {
			printf(" <I>(According to your previous answers, you or your wife should be represented in this next generation)</I>");
		  }
		  printf("</FONT></FONT>\n</P>\n");
			  printf("%s<FONT COLOR=black><P ALIGN=JUSTIFY>",TIMES_10);
			  dones=(int *)malloc((ped->nbindivs)*sizeof(int));
			  for (i=0; i<ped->nbindivs; i++)
				dones[i]=0;
			  for (i=0; i<ped->nbindivs; i++)
			  {
			    if (!dones[i])
			    {
			      ind=ped->indivs[i];
			      if (ind->generation==generation)
			      {
				if (ind->spouse>=0)
				{
#if 0
				  printf("</TR><TR>");
#endif
				  if (ind->sex==0)
				  {
				    print_couple_row2(i+1,ind->spouse+1);
				    dones[i]=1; dones[ind->spouse]=1;
				  }
				  else
				  {
				    print_couple_row2(ind->spouse+1,i+1);
				    dones[i]=1; dones[ind->spouse]=1;
				  }
				}
			      }
			    }
			}
			free(dones);
			printf("</P>");
		  	printf("<TH BGCOLOR=white WIDTH=10%%>&nbsp;</TH>");
			printf("</TR></TABLE><P>");
#if 0
			printf("<CENTER><IMG SRC=%s/gifs/gpsep500.jpg><BR>",GP_HTDOCS_DIR);
#endif
			printf("<CENTER><BR>");
			printf("<A HREF=\"javascript:back()\"><IMG BORDER=0 SRC=%s/gifs/previouspage.jpg ALT=\"Click to go back to the previous page...\"></A>",GP_HTDOCS_DIR);
			printf(" <INPUT TYPE=IMAGE BORDER=0 SRC=%s/gifs/nextpage.jpg ALT=\"Click to proceed to the next generation in your family pedigree...\"> \n",GP_HTDOCS_DIR);
			  printf("</P>");
	#if 0
			  printf("<CENTER><INPUT TYPE=SUBMIT NAME=SUBMIT VALUE=SUBMIT> <INPUT TYPE=RESET NAME=RESET VALUE=RESET>");
	#endif
			  printf("</FORM>");
			}
			else	/* COMPLETED */
			{
			  printf("<BR><CENTER>%s<FONT COLOR=black>Now that you completed the construction of your family pedigree,<BR>please indicate below which individuals are affected by the %s disorder:\n\n", TIMES_10,DB_DISORDER_NAME);
			  print_pheno_row(ped,Ticket);
			}
#if 0
	for (i=0; i<ped->nbindivs; i++)
	{
		printf("%d(%d) %d %d %d %d %d %d<BR>\n",
			ped->indivs[i]->id+1,
			ped->indivs[i]->generation,
			ped->indivs[i]->sex,
			ped->indivs[i]->mother+1,
			ped->indivs[i]->father+1,
			ped->indivs[i]->spouse+1,
			ped->indivs[i]->kid+1,
			ped->indivs[i]->sib+1);
	}
#endif
        	free_pedigree(ped);
		exit(0);
		break;
	    default:
		print_error_page("white", "black", DB_FONT_COLOR, TIMES_12, "Unknown or Invalid Query");
        	exit(0);
        	break;
	}
}
