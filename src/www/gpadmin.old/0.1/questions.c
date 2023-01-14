#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "questions.h"
#include "gpconfig.h"
#include "gphtml.h"
#include "gp_colors.h"

#define DEBUG		0
#define READ_LOCK	0

HTMLFORM *load_form();
void free_question(QUESTION *q);

static char method_str[2][5]={"GET","POST"};
static char modes_str[MAX_MODES][32]= {
	"Yes/No",
	"Radio Buttons",
	"Value/String",
	"Menu",
	"List",
	"Text Area",
	"Multiple Choices" };

void ask_tokens(QUESTION *q)
{
 int i=0;
 int j=0;
 char term[32]={""};
   switch(q->mode)
   {
	case QMODE_MENU:
		sprintf(term,"menu");
		break;
	case QMODE_LIST:
                sprintf(term,"selection list");
                break;
	case QMODE_RADIO:
                sprintf(term,"radio buttons labels");
                break;
	case QMODE_MULTI:
                sprintf(term,"multiple choices");
                break;
   }
   printf("\n<FORM METHOD=GET ACTION=%s_admin>\n",DB_NAME);
   printf("<INPUT TYPE=HIDDEN NAME=ADD_TOKENS VALUE=1></INPUT>\n");
   printf("<INPUT TYPE=HIDDEN NAME=QUESTION VALUE='%s'></INPUT>\n",q->question);
   printf("<INPUT TYPE=HIDDEN NAME=MODE VALUE=%d></INPUT>\n",q->mode);
   printf("<INPUT TYPE=HIDDEN NAME=NBTOKENS VALUE=%d></INPUT>\n",q->nbtokens);
   printf("<INPUT TYPE=HIDDEN NAME=DEFAULT VALUE='%s'></INPUT>\n",q->value);
   printf("%s<FONT COLOR=white>QUESTION FORMULATION:</FONT> <FONT COLOR=%s>%s</FONT></FONT><p>",TIMES_14,bgcol[7],q->question);
   printf("<FONT COLOR=white>%sPlease Enter below the %d descriptions of the possible answers for your %s:<BR>", TIMES_12,q->nbtokens,term);
#if 1
   for (i=0;i<q->nbtokens; i++)
   {
	printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=%s WIDTH=30%%>%s<FONT COLOR=black>Description for Option #%d</FONT></FONT></TH><TH BGCOLOR=white WIDTH=70%%>%s<FONT COLOR=black><INPUT TYPE=TEXT SIZE=48 NAME=TOKEN%d VALUE=''></INPUT></FONT></FONT></TH></TR></TABLE>", bgcol[7], TIMES_10,i+1,TIMES_10,i); 
   }
#endif
   printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=black WIDTH=50%%><INPUT TYPE=SUBMIT NAME=SUBMIT VALUE='Add This Question'></INPUT></TH><TH BGCOLOR=black WIDTH=50%%><INPUT TYPE=RESET NAME=RESET VALUE='Clear Answers'></INPUT></TH></TR></TABLE>");
   printf ("</FONT></FORM>");
   close_cell();
}

int rem_question_from_file(int qi)
{
QUESTION *qq=NULL;
HTMLFORM *f=NULL;
FILE *fin=NULL;
FILE *fout=NULL;
FILE *fdd=NULL;
char strtmp[255]={""};
char line[1024]={""};
time_t Sometime;
int i=0;
int j=0;
int k=0;
  while ((fdd=fopen(DB_QUESTIONS_LOCK,"r"))!=NULL)
  {
        fclose(fdd);
        sleep(1);
  }
  fdd=fopen(DB_QUESTIONS_LOCK,"w");
  fclose(fdd);
  if ((f=(HTMLFORM *)load_form(DB_QUESTIONS_FILE))==NULL)
  {
        printf("Can't load HTMLFORM from %s<BR>",DB_QUESTIONS_FILE);
        return 0;
  }
  Sometime=time(&Sometime);
  f->last_change=Sometime;
  sprintf(strtmp,"%s.%ld",DB_QUESTIONS_FILE,Sometime);
  fin=fopen(DB_QUESTIONS_FILE,"r");
  fout=fopen(strtmp,"w");
  while (!feof(fin))
  {
    fgets(line,1024,fin);
    if (!feof(fin)) fprintf(fout,"%s",line);
  }
  fclose(fout);
  fclose(fin);
  fout=fopen(DB_QUESTIONS_FILE,"w");
  fprintf(fout,">ID\n%ld\n", f->id+1);
  fprintf(fout,">METHOD\n%d\n", f->method);
  fprintf(fout,">LASTCHANGES\n%ld\n", f->last_change);
  fprintf(fout,">NBQUESTIONS\n%d\n", f->nbquestions-1);
  k=0;
  for (i=0; i<f->nbquestions; i++)
  {
    if ((i==qi)&&(k==0))
	k=1;
    else
    {
      qq=f->questions[i];
      fprintf(fout,">QUESTION%d\n%s\n", i-k, qq->question);
      fprintf(fout,">MODE\n%d\n", qq->mode);
      fprintf(fout,">NBTOKENS\n%d\n", qq->nbtokens);
      for (j=0; j<qq->nbtokens; j++)
        fprintf(fout,">TOKEN%d\n%s\n", j, qq->tokens[j]);
      fprintf(fout,">DEFAULT\n%s\n", qq->value);
    }
  }
  fclose(fout);
  remove(DB_QUESTIONS_LOCK);

  
  
}


int add_question_to_file(QUESTION *q)
{
QUESTION *qq=NULL;
HTMLFORM *f=NULL;
FILE *fin=NULL;
FILE *fout=NULL;
FILE *fdd=NULL;
char strtmp[255]={""};
char line[1024]={""};
time_t Sometime;
int i=0; 
int j=0;
  while ((fdd=fopen(DB_QUESTIONS_LOCK,"r"))!=NULL)
  {
        fclose(fdd);
        sleep(1);
  }
  fdd=fopen(DB_QUESTIONS_LOCK,"w");
  fclose(fdd);
  if ((f=(HTMLFORM *)load_form(DB_QUESTIONS_FILE))==NULL)
  {
        printf("Can't load HTMLFORM from %s<BR>",DB_QUESTIONS_FILE);
        return 0;
  }
  Sometime=time(&Sometime);
  f->last_change=Sometime;
  sprintf(strtmp,"%s.%ld",DB_QUESTIONS_FILE,Sometime);
  fin=fopen(DB_QUESTIONS_FILE,"r");
  fout=fopen(strtmp,"w");
  while (!feof(fin))
  {
    fgets(line,1024,fin);
    if (!feof(fin)) fprintf(fout,"%s",line);
  }
  fclose(fout);
  fclose(fin);
  f->id=f->id+1;
  q->id=f->nbquestions;
  f->questions=(QUESTION **)realloc(f->questions, (q->id+1)*sizeof(QUESTION *));
  f->questions[q->id]=(QUESTION *)q;
  f->nbquestions=f->nbquestions+1;
  fout=fopen(DB_QUESTIONS_FILE,"w");
  fprintf(fout,">ID\n%ld\n", f->id);
  fprintf(fout,">METHOD\n%d\n", f->method);
  fprintf(fout,">LASTCHANGES\n%ld\n", f->last_change);
  fprintf(fout,">NBQUESTIONS\n%d\n", f->nbquestions);
  for (i=0; i<f->nbquestions; i++)
  {
    qq=f->questions[i];
    fprintf(fout,">QUESTION%d\n%s\n", i, qq->question);
    fprintf(fout,">MODE\n%d\n", qq->mode);
    fprintf(fout,">NBTOKENS\n%d\n", qq->nbtokens);
    for (j=0; j<qq->nbtokens; j++)
	fprintf(fout,">TOKEN%d\n%s\n", j, qq->tokens[j]);
    fprintf(fout,">DEFAULT\n%s\n", qq->value);
  }
  fclose(fout);
  remove(DB_QUESTIONS_LOCK);
}

void show_new_question(int qi)
{
QUESTION *q=NULL;

  printf("\n<FORM METHOD=GET ACTION=%s_admin>",DB_NAME);
  printf("<INPUT TYPE=HIDDEN NAME=ADD_QUESTION VALUE=%d></INPUT>\n",qi);
  printf("%sPlease Formulate your request or question below:<BR><INPUT TYPE=TEXT SIZE=64 NAME=QUESTION VALUE=''></INPUT></FONT>\n",TIMES_10);
  printf("%s<P>Select Specific Information for your new question below:<BR></FONT>\n",TIMES_10);
  printf("<TABLE BORDER=0 WIDTH=100%%>");
  printf("<TR>\n");
  printf("<TH BGCOLOR=%s WIDTH=10%%>%s<FONT COLOR=black>Select</FONT></FONT></TH>", bgcol[7],TIMES_10);
  printf("<TH BGCOLOR=%s WIDTH=20%%>%s<FONT COLOR=black>Question Type</FONT></FONT></TH>", bgcol[7],TIMES_10);
  printf("<TH BGCOLOR=%s WIDTH=35%%>%s<FONT COLOR=black>Specific Options</FONT></FONT></TH>", bgcol[7],TIMES_10);
  printf("<TH BGCOLOR=%s WIDTH=35%%>%s<FONT COLOR=black>Default Answers/Others</FONT></FONT></TH>", bgcol[7],TIMES_10);
  printf("</TR>\n"); 
  printf("<TR>\n");	/* YES_NO */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE VALUE=0 CHECKED></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Yes or No</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%>%s<FONT COLOR=black>---</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%>%s<FONT COLOR=black><SELECT TYPE=MENU NAME=DEFAULT><OPTION VALUE=1>Yes</OPTION><OPTION VALUE=0>No</OPTION></SELECT></FONT></FONT></TH>", TIMES_10);
  printf("</TR>\n");
  printf("<TR>\n");     /* RADIO */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE VALUE=1></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Radio Buttons</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%><FONT COLOR=black>%sNumber of buttons</TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT NAME=NBTOKENS SIZE=6 VALUE='0'></INPUT></FONT></FONT></TH></TR></TABLE></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>Default button</FONT></FONT></TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT VALUE='0' SIZE=6 NAME=DEFAULT></INPUT></TH></TR></TABLE></TH>", TIMES_10);
  printf("</TR>\n");
  printf("<TR>\n");     /* VALUE */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE VALUE=2></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Value or String</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%><FONT COLOR=black>%sNumber of characters</TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT NAME=NBTOKENS SIZE=6 VALUE='0'></INPUT></FONT></FONT></TH></TR></TABLE></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>Default value</FONT></FONT></TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT VALUE='0' SIZE=6 NAME=DEFAULT></INPUT></TH></TR></TABLE></TH>", TIMES_10);
  printf("</TR>\n");
  printf("<TR>\n");     /* MENU */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE NAME=NBTOKENS VALUE=3></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Menu</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%><FONT COLOR=black>%sNumber of options</TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT SIZE=6 NAME=NBTOKENS VALUE='0'></INPUT></FONT></FONT></TH></TR></TABLE></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>Default Option</FONT></FONT></TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT VALUE='0' SIZE=6 NAME=DEFAULT></INPUT></TH></TR></TABLE></TH>", TIMES_10);
  printf("</TR>\n");
  printf("<TR>\n");     /* LIST */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE VALUE=4></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Selection List</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%><FONT COLOR=black>%sNumber choices</TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT NAME=NBTOKENS SIZE=6 VALUE='0'></INPUT></FONT></FONT></TH></TR></TABLE></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>Default choice</FONT></FONT></TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT VALUE='0' SIZE=6 NAME=DEFAULT></INPUT></TH></TR></TABLE></TH>", TIMES_10);
  printf("</TR>\n");
   printf("<TR>\n");     /* TEXTAREA */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE VALUE=5></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Text Area</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>Numbers of lines </TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT NAME=NBTOKENS SIZE=6 VALUE='3'></INPUT></FONT></FONT></TH></TR></TABLE></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%% HALIGN=right><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>and columns</TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT VALUE='32' SIZE=6 NAME=DEFAULT></INPUT></FONT></FONT></TH></TR></TABLE></TH>", TIMES_10);
  printf("</TR>\n");
   printf("<TR>\n");     /* LIST */
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black><INPUT TYPE=RADIO NAME=MODE VALUE=6></INPUT></FONT></FONT></TH>", TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>Multiple Choices</FONT></FONT></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%><FONT COLOR=black>%sNumber of choices</TH><TH BGCOLOR=white WIDTH=50%%> <INPUT TYPE=TEXT NAME=NBTOKENS SIZE=6 VALUE='1'></INPUT></FONT></FONT></TH></TR></TABLE></TH>",TIMES_10);
  printf("<TH BGCOLOR=white WIDTH=35%%><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>Default choice</FONT></FONT></TH><TH BGCOLOR=white WIDTH=50%%><INPUT TYPE=TEXT VALUE='0' SIZE=6 NAME=DEFAULT></INPUT></TH></TR></TABLE></TH>", TIMES_10);
  printf("</TR>\n");
  printf("</TABLE>\n");
  printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=black WIDTH=50%%><INPUT TYPE=SUBMIT NAME=SUBMIT VALUE='Add To Questionnaire'</A></INPUT></TH><TH BGCOLOR=black WIDTH=50%%><BR><INPUT TYPE=RESET NAME=RESET VALUE='Reset This Form'></INPUT></FORM></TH></TR></TABLE>");

}

print_htmltoken(QUESTION *q)
{
 int i=0;
 int value=0;
 int value1=0;
 float cell=1.0;
  switch (q->mode)
  {
     case QMODE_YESNO:
  	  sscanf(q->value,"%d",&value);
	   printf("<SELECT TYPE=MENU NAME=QUESTION%d><OPTION VALUE=%d>%s%s</FONT></OPTION><OPTION VALUE=%d>%s%s</FONT></OPTION><SELECT>",
		q->id, value,TIMES_10, q->tokens[value],
		(value+1)%2,TIMES_10, q->tokens[(value+1)%2]);
	break;

     case QMODE_RADIO:
  	sscanf(q->value,"%d",&value);
	if (q->nbtokens!=0)
		cell=1.0/(float)(q->nbtokens);
	printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
	for (i=0;i<q->nbtokens; i++)
	{
	   printf("<TH BGCOLOR=white WIDTH=%d%%>", (int)(100.0*cell));
           printf("%s<FONT COLOR=black>%s</FONT></FONT>",TIMES_10,q->tokens[i]);
           printf("</TH>");
	}
	printf("</TR><TR>");
	for (i=0;i<q->nbtokens; i++)
	{
	   printf("<TH BGCOLOR=white WIDTH=%d%%>", (int)(100.0*cell));
	   if (value==i)
	      printf("<INPUT TYPE=RADIO NAME=QUESTION%d VALUE=%d CHECKED></INPUT>", q->id, i);
	   else
	   printf("<INPUT TYPE=RADIO NAME=QUESTION%d VALUE=%d></INPUT>",q->id, i);
	   printf("</TH>");
	}
	printf("</TR></TABLE>");
	break;

     case QMODE_VALUE:
	/* Token is used for size of text */
	printf ("%s<INPUT TYPE=TEXT NAME=QUESTION%d VALUE='%s' SIZE=%s WRAP=VIRTUAL></INPUT></FONT>",
		TIMES_10, q->id, q->value,  q->tokens[0]);
	break;

     case QMODE_MENU:
     case QMODE_LIST:
     case QMODE_MULTI:
  	sscanf(q->value,"%d",&value);
	if (q->mode==QMODE_MENU)
	  printf("<SELECT TYPE=MENU NAME=QUESTION%d>",q->id);
	else
	   if (q->mode==QMODE_LIST)
		printf("<SELECT TYPE=LIST SIZE=%d NAME=QUESTION%d>",q->nbtokens,q->id);
	   else
		printf("<SELECT MULTIPLE TYPE=LIST SIZE=%d NAME=QUESTION%d>",q->nbtokens,q->id);
	for (i=0;i<q->nbtokens; i++)
        {
           if (value==i)
		printf("<OPTION VALUE=%d SELECTED>%s%s</FONT></OPTION>", i,TIMES_10,q->tokens[i]);
           else
		printf("<OPTION VALUE=%d>%s%s</FONT></OPTION>", i,TIMES_10,q->tokens[i]);
        }
	printf("</SELECT>");
	break;
     case QMODE_TEXTAREA:
  	sscanf(q->tokens[0],"%d",&value);
  	sscanf(q->value,"%d",&value1);
	printf("%s<FONT COLOR=black>%s</FONT><BR>",TIMES_10,q->question);
	printf("<TEXTAREA NAME=QUESTION%d ROWS=%d COLS=%d></TEXTAREA></FONT>",q->id,value,value1);
	break;
  } 
}


QUESTION *create_question()
{
 QUESTION *q=NULL;
  if ((q=(QUESTION *)malloc(sizeof(QUESTION)))==NULL)
	return (QUESTION *)NULL;
  q->question=(char *)NULL;
  q->mode=(int)0;
  q->nbtokens=(int)0;
  q->tokens=(char **)NULL;
  q->value=(int) 0;
  return (QUESTION *)q;
}

void free_question(QUESTION *q)
{
  int i=0;
  if (q->question!=NULL) free(q->question);
  for (i=0; i<q->nbtokens; i++)
	if (q->tokens[i]!=NULL)
		free(q->tokens[i]);
  if (q->tokens!=NULL) free(q->tokens);
  if (q->value!=NULL) free(q->value);
  if (q!=NULL) free(q);
}

HTMLFORM *create_htmlform()
{
 HTMLFORM *f=NULL;
  f=(HTMLFORM *)malloc(sizeof(HTMLFORM));
  f->id=0;
  f->action=(char *)NULL;
  f->method=(int)0;
  f->nbquestions=0;
  f->questions=(QUESTION **)NULL;
  return (HTMLFORM *)f;
}

void free_form(HTMLFORM *f)
{
 QUESTION *q=NULL;
 int i=0;
 if (f->action!=NULL) free(f->action);
 for (i=0;i<f->nbquestions;i++)
	free_question(f->questions[i]);
 if (f->questions!=NULL) free(f->questions);
 if (f!=NULL) free(f);
}

HTMLFORM *load_form()
{
 HTMLFORM *f=NULL;
 QUESTION *q=NULL;
 FILE *fd=NULL;
 FILE *fdd=NULL;
 char line[1024];
 char strtmp[255];
 time_t Sometime;
 int i=0;
 int j=0;

#if READ_LOCK
  while ((fdd=fopen(DB_QUESTIONS_LOCK,"r"))!=NULL)
  {
	fclose(fdd);
	sleep(1);
  }
  fdd=fopen(DB_QUESTIONS_LOCK,"w");
  fclose(fdd);
#endif
  if ((fd=fopen(DB_QUESTIONS_FILE,"r"))==NULL)
  {
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	return (HTMLFORM *)NULL;
  }
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  if (strncmp(line,">ID",3)!=0)
  {
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	return NULL;
  }
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  sscanf(line,"%d",&i);
  f=(HTMLFORM *)create_htmlform();
  f->id=(int)i;
  sprintf(strtmp,"%s_admin",DB_NAME);
  f->action=(char *)malloc(strlen(strtmp)*sizeof(char));
  strcpy(f->action,strtmp);	/* Set FORM ACTION */
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  if (strncmp(line,">METHOD",7)!=0)	/* Get METHOD */
  {
#if DEBUG
	printf("ERR_METHOD\n");
#endif
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	free_form(f);
	return NULL;
  }
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  sscanf(line,"%d",&i);
  f->method=(int)i;			/* Set METHOD */
#if DEBUG
  printf("METHOD=%d<BR>\n",f->method);
#endif
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  if (strncmp(line,">LASTCHANGES",12)!=0)	/* Get LASTCHANGES */
  {
#if DEBUG
	printf("ERR_LASTCHANGES\n");
#endif
        fclose(fd);
#if READ_LOCK
        remove(DB_QUESTIONS_LOCK);
#endif
        free_form(f);
        return NULL;
  }
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  sscanf(line,"%ld",&Sometime);
  f->last_change=(time_t)Sometime;		/* Set LASTCHANGES */
#if DEBUG
  strcpy(strtmp,ctime(&Sometime));
  strtmp[strlen(strtmp)-1]='\0';
  printf("LAST CHANGES: %s<br>\n",strtmp);
#endif
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  if (strncmp(line,">NBQUESTIONS",12)!=0)	/* Get NBQUESTIONS */
  {
#if DEBUG
	printf("ERR_NBQUESTIONS\n");
#endif
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
        free_form(f);
        return NULL;
  }
  fgets(line,1024,fd);
  line[strlen(line)-1]='\0';
  sscanf(line,"%d",&i);
  f->nbquestions=(int)i;			/* Set NBQUESTIONS */
#if DEBUG
  printf("NBQUESTIONS=%d<BR>\n",f->nbquestions);
#endif
  f->questions=(QUESTION **)malloc(i*sizeof(QUESTION *));
  for (i=0; i<f->nbquestions; i++)
  {
     sprintf(strtmp,">QUESTION%d",i);
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     if (strncmp(line,strtmp,strlen(strtmp))!=0)  /* Get QUESTION? */
     {
#if DEBUG
	printf("ERR_%s\n",strtmp+1);
#endif
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	free_form(f);
	return NULL;
     }
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     f->questions[i]=create_question();
     q= (QUESTION *) f->questions[i];
     q->id=i;
     q->question=(char *)malloc(strlen(line)*sizeof(char));
     strcpy(q->question,line);
#if DEBUG
     printf("QUESTION%d=[%s]<BR>",i,q->question);
#endif
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     if (strncmp(line,">MODE",5)!=0)
     {
#if DEBUG
	printf("ERR_MODE\n");
#endif
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	free_form(f);
	return NULL;
     }
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     sscanf(line,"%d",&j);
     q->mode=(int)j;
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     if (strncmp(line,">NBTOKENS",9)!=0)
     {
#if DEBUG
	printf("ERR_NBTOKENS\n");
#endif
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	free_form(f);
        return NULL;
     }
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     sscanf(line,"%d",&j);
     q->nbtokens=(int)j;
#if DEBUG
     printf("NBTOKENS=%d<br>\n",j);
#endif
     q->tokens=(char **)malloc(j*sizeof(char *));
     for (j=0; j< q->nbtokens; j++)
     {
	sprintf(strtmp,">TOKEN%d",j);
        fgets(line,1024,fd);
        line[strlen(line)-1]='\0';
	if (strncmp(line,strtmp,strlen(strtmp))!=0)
 	{
#if DEBUG
	printf("ERR_%s\n",strtmp+1);
#endif
	   fclose(fd);
#if READ_LOCK
	   remove(DB_QUESTIONS_LOCK);
#endif
	   free_form(f);
	   return NULL;
	}
        fgets(line,1024,fd);
        line[strlen(line)-1]='\0';
	q->tokens[j]=(char *)malloc(strlen(line)*sizeof(char));
	strcpy(q->tokens[j],line);
#if DEBUG
        printf("TOKEN%d=[%s]<br>\n",j,line);
#endif
     }
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     if (strncmp(line,">DEFAULT",8)!=0)
     {
#if DEBUG
	printf("ERR_DEFAULT\n");
#endif
	fclose(fd);
#if READ_LOCK
	remove(DB_QUESTIONS_LOCK);
#endif
	free_form(f);
	return NULL;
     }
     fgets(line,1024,fd);
     line[strlen(line)-1]='\0';
     q->value=(char *)malloc(strlen(line)*sizeof(char));
     strcpy(q->value, line);
#if DEBUG
     printf("DEFAULT VALUE=[%s]<br>\n",q->value);
#endif
  }
  fclose(fd);
#if READ_LOCK
  remove(DB_QUESTIONS_LOCK);
#endif
  return (HTMLFORM *)f;
}

int show_questions()
{
HTMLFORM *f=NULL;
QUESTION *q=NULL;
int i=0;
char strtmp[128]={""};
time_t Sometime;
 if ((f=load_form(DB_QUESTIONS_FILE))==NULL)
 {
 	printf("Can't load HTMLFORM from %s<BR>",DB_QUESTIONS_FILE);
	return;
 }
 /*printf("<FORM NAME=%s>",f->action);*/
 Sometime=f->last_change;
 strcpy(strtmp,ctime(&Sometime));
 strtmp[strlen(strtmp)-1]='\0';
 printf("(Last Modified: %s)<BR>",strtmp);
 printf("<FORM>",f->action);
 make_new_cell();
 if (f->nbquestions>0)
 {
    printf("<TABLE BORDER=0 WIDTH=100%%>\n");
    printf("<TR>");
    printf("<TH BGCOLOR=%s WIDTH=5%%>%s<FONT COLOR=black>Question</FONT></FONT></TH>",bgcol[7],TIMES_12);
    printf("<TH BGCOLOR=%s WIDTH=40%%>%s<FONT COLOR=black>Formulation</FONT></FONT></TH>",bgcol[7],TIMES_12);
    printf("<TH BGCOLOR=%s WIDTH=15%%>%s<FONT COLOR=black>Question Type</FONT></FONT></TH>",bgcol[7],TIMES_12);
    printf("<TH BGCOLOR=%s WIDTH=30%%>%s<FONT COLOR=black>Answer Options</FONT></FONT></TH>",bgcol[7],TIMES_12);
    printf("<TH BGCOLOR=%s WIDTH=10%%>%s<FONT COLOR=black>&nbsp;</FONT></FONT></TH>",bgcol[7],TIMES_12);
    printf("</TR>");
    
 }
 for (i=0; i< f->nbquestions; i++)
 {
    q= f->questions[i];
    printf("<TR>");
    printf("<TH BGCOLOR=white WIDTH=5%%>%s<FONT COLOR=black>%d</FONT></FONT></TH>",TIMES_12,i+1);
    printf("<TH BGCOLOR=white WIDTH=40%%>%s<FONT COLOR=black>%s</FONT></FONT></TH>",TIMES_12,q->question);
    printf("<TH BGCOLOR=white WIDTH=15%%>%s</b><FONT COLOR=black>%s</FONT></FONT></TH>",TIMES_12,modes_str[q->mode]);
    printf("<TH BGCOLOR=white WIDTH=30%%>%s<FONT COLOR=black><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white>",
		TIMES_12);
    print_htmltoken(q);
    printf("</TH></TR></TABLE></FONT></FONT></TH>\n");
    printf("<TH BGCOLOR=white WIDTH=10%%>%s<A HREF=%s?REMOVE_QUESTION=%d ALT='Remove Question%d'><FONT COLOR=blue>Remove</FONT></A></FONT></TH>",TIMES_12,f->action,q->id,q->id);
    printf("</TR>");
    
 }
 if (f->nbquestions>0)
    printf("</TABLE>");
 printf("</FORM>");
 i=f->nbquestions;
 free_form(f);
 close_cell();
 return i;
}
