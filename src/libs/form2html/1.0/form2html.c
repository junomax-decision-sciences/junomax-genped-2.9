#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "form2html.h"

#define VERBOSE_PARSER	0

char tokens[MAX_TOKENS][255] = {
/*0*/	"START_RADIO",
/*1*/	"RADIO_TITLE=",		/* The paragraph above the radio button */
/*2*/	"RADIO=",
/*3*/	"RADIO_VALUE=",		/* There can be several */
/*4*/	"RADIO_SVALUE=",	/* There can be only one */
/*5+*/  "unused","unused","unused","unused",
/*9*/	"END_RADIO",

/*10*/	"START_CHECKBOX",
/*11*/	"CHECKBOX_TITLE=",
/*12*/	"CHECKBOX=",
/*13*/	"CHECKBOX_VALUE=",	/* Unchecked button by default */
/*14*/	"CHECKBOX_SVALUE=",	/* Checked button by default */
/*15+*/ "unused","unused","unused","unused",
/*19*/	"END_CHECKBOX",

/*20*/	"START_MENU",
/*21*/	"MENU_TITLE=",
/*22*/	"MENU=",
/*23*/	"MENU_VALUE=",		/* Non Default Option */
/*24*/	"MENU_SVALUE=",		/* Default Option (only 1) */
/*25*/	"MENU_SIZE=",
/*26+*/  "unused","unused","unused",
/*29*/	"END_MENU",

/*30*/	"START_MULTIPLE",
/*31*/	"MULTIPLE_TITLE=",
/*32*/	"MULTIPLE=",
/*33*/	"MULTIPLE_VALUE=",
/*34*/	"MULTIPLE_SVALUE=",	/* Selected Value (can be more than 1) */
/*35*/	"MULTIPLE_SIZE=",
/*36+*/  "unused","unused","unused",
/*39*/	"END_MULTIPLE",

/*40*/	"START_TEXTAREA",
/*41*/	"TEXTAREA_TITLE=",
/*42*/	"TEXTAREA=",
/*43*/	"TEXTAREA_VALUE=",	/* Default text in the textarea */
/*44*/	"TEXTAREA_COLS=",
/*45*/	"TEXTAREA_ROWS=",
/*46+*/  "unused","unused","unused",
/*49*/	"END_TEXTAREA",


/*50*/	"START_TEXT",
/*51*/	"TEXT_TITLE=",
/*52*/	"TEXT=",
/*53*/	"TEXT_VALUE=",		/* Default text in the box */
/*54*/	"TEXT_SIZE=",
/*55+*/  "unused","unused","unused","unused",
/*59*/	"END_TEXT",

/*60*/	"START_FILE",
/*61*/	"FILE=",
/*62*/	"FILE_VALUE=",		/* Default filename to upload */
/*63*/	"FILE_FILTER=",
/*64*/	"FILE_MIMETYPE=",
/*65+*/  "unused","unused","unused","unused",
/*69*/	"END_FILE",

/*70*/	"START_HTML",	/* With this one can put some HTML formatting */
/*71+*/	"unused","unused","unused","unused","unused","unused","unused","unused",
/*79*/	"END_HTML",

#if 0
/*80*/	"START_IMAGE",
/*81*/	"IMAGE_TITLE",
/*82*/	"IMAGE=",	/* The IMAGE NAME */
/*83*/	"IMAGE_SRC",	/* The IMAGE URL */
/*84*/	"IMAGE_ALIGN",	/* The IMAGE ALIGN */
/*85+*/ "unused","unused","unused","unused",
/*89*/	"END_IMAGE"
#endif
};

int get_token(char *line)
{
int i=0;
  for (i=0; i<MAX_TOKENS; i++)
  {
    if (strncmp(line,tokens[i],strlen(tokens[i]))==0)
    {
#if VERBOSE_PARSER
  	printf("LINE=[%s] TOKEN=%d\n",line,i);
#endif
	return i;
    }
  }
#if VERBOSE_PARSER
  printf("LINE=[%s] UNKNOWN TOKEN\n",line);
#endif
  return -1;
}

void form2html(char *filename)
{
FILE *fd=NULL;
char line[1024]={""};
int token=-1;
int token2=-1;
char tokstr[32000]={""};
char sizestr[255]={""};
char rowsstr[255]={""};
char colsstr[255]={""};
char tokenname[255]={""};
  if (filename==NULL) return;
  if ((fd=fopen(filename,"r"))==NULL)
  {
    perror(filename);
    return;
  }
  while (!feof(fd))
  {
    if (fgets(line,1023,fd)!=NULL)
    {
	line[strlen(line)-1]='\0';
	token=get_token(line);
	switch(token)
	{
	  case 0:	/******************* START_RADIO **********************/
		strcpy(tokenname,"");
		while (strncmp(line,tokens[9],strlen(tokens[9]))!=0)
		{
		  if (fgets(line,1023,fd)!=NULL)
		  {
		     line[strlen(line)-1]='\0';
		     token2=get_token(line);  
		     switch(token2)
		     {
			case 1:		/*** RADIO_TITLE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("RADIO_TITLE must precede the RADIO= token.\n");
			     exit(1);
  		          }
			  printf("%s<BR>\n",line+strlen(tokens[token2]));
			  break;
			case 2:		/*** RADIO= ***/
			  strcpy(tokenname,line+strlen(tokens[token2]));
			  printf("%s ",tokenname);
			  break;
			case 3:		/*** RADIO_VALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the RADIO= token.\n");
			     exit(1);
  		          }
			  printf(" <INPUT TYPE=RADIO NAME=\"%s\" VALUE=\"%s\"> %s \n",tokenname,line+strlen(tokens[token2]), line+strlen(tokens[token2]));
			  break;
			case 4:		/*** RADIO_SVALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the RADIO= token.\n");
			     exit(1);
  		          }
			  printf(" <INPUT TYPE=RADIO NAME=\"%s\" VALUE=\"%s\" CHECKED> %s \n",tokenname,line+strlen(tokens[token2]), line+strlen(tokens[token2]));
			  break;
		     }
		  }
		}
		break;
	  case 10:	/******************* START_CHECKBOX *******************/
		strcpy(tokenname,"");
		while (strncmp(line,tokens[19],strlen(tokens[19]))!=0)
		{
		  if (fgets(line,1023,fd)!=NULL)
		  {
		     line[strlen(line)-1]='\0';
		     token2=get_token(line);  
		     switch(token2)
		     {
			case 11:		/*** CHECKBOX_TITLE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("CHECKBOX_TITLE must precede the CHECKBOX= token.\n");
			     exit(1);
  		          }
			  printf("%s<BR>\n",line+strlen(tokens[token2]));
			  break;
			case 12:		/*** CHECKBOX= ***/
			  strcpy(tokenname,line+strlen(tokens[token2]));
			  printf("%s ",tokenname);
			  break;
			case 13:		/*** CHECKBOX_VALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the CHECKBOX= token.\n");
			     exit(1);
  		          }
			  printf("<INPUT TYPE=CHECKBOX NAME=\"%s\" VALUE=\"%s\"> %s\n", tokenname, line+strlen(tokens[token2]),line+strlen(tokens[token2]));
			  break;
			case 14:		/*** CHECKBOX_SVALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the CHECKBOX= token.\n");
			     exit(1);
  		          }
			  printf("<INPUT TYPE=CHECKBOX NAME=\"%s\" VALUE=\"%s\" CHECKED> %s\n", tokenname, line+strlen(tokens[token2]),line+strlen(tokens[token2]));
			  break;
		     }
		  }
		}
		break;
	  case 20:	/******************* START_MENU ***********************/
		strcpy(tokenname,"");
		strcpy(tokstr,"");
		while (strncmp(line,tokens[29],strlen(tokens[29]))!=0)
		{
		  if (fgets(line,1023,fd)!=NULL)
		  {
		     line[strlen(line)-1]='\0';
		     token2=get_token(line);  
		     switch(token2)
		     {
			case 21:		/*** MENU_TITLE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("MENU_TITLE must precede the MENU= token.\n");
			     exit(1);
  		          }
			  printf("%s<BR>\n",line+strlen(tokens[token2]));
			  break;
			case 22:		/*** MENU= ***/
			  strcpy(tokenname,line+strlen(tokens[token2]));
			  printf("%s ",tokenname);
			  sprintf(tokstr,"<SELECT TYPE=MENU NAME=\"%s\">\n",tokenname);
			  break;
			case 23:		/*** MENU_VALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the MENU= token.\n");
			     exit(1);
  		          }
			  sprintf(tokstr,"%s<OPTION VALUE=\"%s\">%s</OPTION>\n",
				 tokstr,
				 line+strlen(tokens[token2]),
				 line+strlen(tokens[token2]));
			  break;
			case 24:		/*** MENU_SVALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the MENU= token.\n");
			     exit(1);
  		          }
			  sprintf(tokstr,"%s<OPTION VALUE=\"%s\" SELECTED>%s</OPTION>\n",
				 tokstr,
				 line+strlen(tokens[token2]),
				 line+strlen(tokens[token2]));
			  break;
			case 29:
			  if (strlen(tokstr)>0)
			  {
			     printf("%s",tokstr);
			     printf("</SELECT>\n");
			  }
			  break;
		     }
		  }
		}
		break;
	  case 30:	/******************* START_MULTIPLE *******************/
		strcpy(tokenname,"");
		strcpy(tokstr,"");
		strcpy(sizestr,"");
		while (strncmp(line,tokens[39],strlen(tokens[39]))!=0)
		{
		  if (fgets(line,1023,fd)!=NULL)
		  {
		     line[strlen(line)-1]='\0';
		     token2=get_token(line);  
		     switch(token2)
		     {
			case 31:		/*** MULTIPLE_TITLE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("MULTIPLE_TITLE must precede the MULTIPLE= token.\n");
			     exit(1);
  		          }
			  printf("%s<BR>\n",line+strlen(tokens[token2]));
			  break;
			case 32:		/*** MULTIPLE= ***/
			  strcpy(tokenname,line+strlen(tokens[token2]));
			  printf("%s ",tokenname);
			  sprintf(tokstr,"<SELECT NAME=\"%s\"",tokenname);
			  if (strlen(sizestr)>0)
			  {
				sprintf(tokstr,"%s SIZE=%s",tokstr,sizestr);
			  }
			  strcat(tokstr," MULTIPLE>");
			  break;
			case 33:		/*** MULTIPLE_VALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the MULTIPLE= token.\n");
			     exit(1);
  		          }
			  sprintf(tokstr,"%s <OPTION VALUE=\"%s\">%s</OPTION>\n",
				 tokstr,
				 line+strlen(tokens[token2]),
				 line+strlen(tokens[token2]));
			  break;
			case 34:		/*** MULTIPLE_SVALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the MULTIPLE= token.\n");
			     exit(1);
  		          }
			  sprintf(tokstr,"%s<OPTION VALUE=\"%s\" SELECTED>%s</OPTION>\n",
				 tokstr,
				 line+strlen(tokens[token2]),
				 line+strlen(tokens[token2]));
			  break;
			case 35:
			  if (strlen(tokenname)>0)
                          {
                             printf("MULTIPLE_SIZE must precede MULTIPLE= token.\n");
                             exit(1);
                          }
			  strcpy(sizestr,line+strlen(tokens[token2]));
			  break;
			case 39:
			  if (strlen(tokstr)>0)
			  {
			     printf("%s",tokstr);
			     printf("</SELECT>\n");
			  }
			  break;
		     }
		  }
		}
		break;
	  case 40:	/******************* START_TEXTAREA ********************/
		strcpy(tokenname,"");
		strcpy(colsstr,"");
		strcpy(rowsstr,"");
		while (strncmp(line,tokens[49],strlen(tokens[49]))!=0)
		{
		  if (fgets(line,1023,fd)!=NULL)
		  {
		     line[strlen(line)-1]='\0';
		     token2=get_token(line);  
		     switch(token2)
		     {
			case 41:		/*** TEXTAREA_TITLE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("TEXTAREA_TITLE must precede the TEXTAREA= token.\n");
			     exit(1);
  		          }
			  printf("%s<BR>\n",line+strlen(tokens[token2]));
			  break;
			case 42:		/*** TEXTAREA= ***/
			  strcpy(tokenname,line+strlen(tokens[token2]));
			  printf("%s<BR>\n",tokenname);
			  break;
			case 43:		/*** TEXTAREA_VALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the TEXTAREA= token.\n");
			     exit(1);
  		          }
			  printf("<TEXTAREA NAME=\"%s\" VALUE=\"%s\"", tokenname, line+strlen(tokens[token2]));
			  if (strlen(rowsstr)>0)
				printf(" ROWS=%s",rowsstr);
			  if (strlen(colsstr)>0)
				printf(" COLS=%s",colsstr);
			  printf(">%s</TEXTAREA>\n",line+strlen(tokens[token2]));
			  break;
			case 44:		/*** TEXT_SIZE ***/
			  if (strlen(tokenname)==0)
                          {
                             printf("You mustfirst set the TEXTAREA= token before
 using TEXTAREA_COLS.\n");
                             exit(1);
                          }
                          sprintf(colsstr,"%s",line+strlen(tokens[token2]));
			  break;
			case 45:
			  if (strlen(tokenname)==0)
                          {
                             printf("You mustfirst set the TEXTAREA= token before using TEXTAREA_ROWS.\n");
                             exit(1);
                          }
			  sprintf(rowsstr,"%s",line+strlen(tokens[token2]));
                          break;
		     }
		  }
		}
		break;
	  case 50:	/******************* START_TEXT **********************/
		strcpy(tokenname,"");
		strcpy(sizestr,"");
		while (strncmp(line,tokens[59],strlen(tokens[59]))!=0)
		{
		  if (fgets(line,1023,fd)!=NULL)
		  {
		     line[strlen(line)-1]='\0';
		     token2=get_token(line);  
		     switch(token2)
		     {
			case 51:		/*** TEXT_TITLE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("TEXT_TITLE must precede the TEXT= token.\n");
			     exit(1);
  		          }
			  printf("%s<BR>\n",line+strlen(tokens[token2]));
			  break;
			case 52:		/*** TEXT= ***/
			  strcpy(tokenname,line+strlen(tokens[token2]));
			  printf("%s ",tokenname);
			  break;
			case 53:		/*** TEXT_VALUE ***/
			  if (strlen(tokenname)==0)
			  {
			     printf("You must first set the TEXT= token.\n");
			     exit(1);
  		          }
			  if (strlen(sizestr)>0)
			    printf(" <INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\" SIZE=%s>\n", tokenname, line+strlen(tokens[token2]),sizestr);
			  else
			    printf(" <INPUT TYPE=TEXT NAME=\"%s\" VALUE=\"%s\">\n", tokenname, line+strlen(tokens[token2]));
			  break;
			case 54:		/*** TEXT_SIZE ***/
			  if (strlen(tokenname)>0)
			  {
			     printf("TEXT_SIZE must first set the TEXT= token.\n");
			     exit(1);
  		          }
			  sprintf(sizestr,"%s",line+strlen(tokens[token2]));
			  break;
		     }
		  }
		}
		break;
	  case 60:	/******************* START_FILE ************************/
		break;
	  case 70:	/******************* START_HTML ************************/
                while (strncmp(line,tokens[79],strlen(tokens[79]))!=0)
                {
                  if (fgets(line,1023,fd)!=NULL)
                  {
                     line[strlen(line)-1]='\0';
		     if (strncmp(line,tokens[79],strlen(tokens[79]))!=0)
                     	printf("%s\n",line);
	          }
		}
		break;
	  default:	/* Default is to ignore the line if unrecognized */
		break;
	}
    }
  }
  
}
