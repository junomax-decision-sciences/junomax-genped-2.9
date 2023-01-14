#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <fcntl.h>
#include <sys/file.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "gphtml.h"
#include "gpadmin.h"
#include "mainpage.h"
#include "gpconfig.h"
#include "questions.h"


exitpage(int errormode)
{
   switch (errormode)
   {
	case INCORRECT_ENTRIES:
		printf ("<br><FONT COLOR=yellow>Problem with the page.<br>Please contact an administrator</FONT><p>\n");
		break;
   }
   printf("<br><br><br><FONT COLOR=#88c8c0>Copyright &copy;1997</FONT> <A HREF=http://www.lexgen.com><FONT COLOR=white>Lexicon Genetics Inc.</FONT></A><p>");
   printf("</BODY></HTML>");
   exit(1);
}

extern void list_pending();
extern void examine_pending();

char ob_user[30]={""};
char ob_host[64]={""};
char *c2=NULL;
char *c=NULL;
int cl=0;

typedef struct {
    char name[256];
    char val[4096];
} entry;
static entry entries[4000];


main(int argc, char *argv[]) {
char strtmp[255]={""};
char no_chears=0;
long ped_id=0;	/* Some pedigree id */
long ticket_id=0;	/* Some pedigree ticket */
int pos=0;
int count=0;
int x=0, m=0;
char dbn[64]={""};
int i=0;
int j=0;
int k=0;
QUESTION *q=NULL;
char done=0;

	while (argv[0][pos]!='_') pos++;
	strncpy(dbn,argv[0],pos);
	dbn[pos]='\0';

	config_get_lex(dbn);
 	sprintf(strtmp,"GenPed Administrator Web Interface %s",PROG_VERSION);
        setbuf(stdout,NULL);
        count=0;
        if (getenv("REMOTE_USER")!=NULL)
                strcpy(ob_user,getenv("REMOTE_USER"));
        else strcpy(ob_user,"unknown");
        if (getenv("REMOTE_HOST")!=NULL)
        {
              if (strcpy(ob_host,(char *)getenv("REMOTE_HOST"))==NULL)
                 strcpy(ob_host,"unknown");
        }
	if (is_admin_user(ob_user)==0)
	{
	   print_top(0,0,strtmp);
	   printf("<BR><BR>");
	   make_sub_cell();
	   printf("<BR><CENTER>%s<FONT COLOR=white>User [%s] is not authorized to use the GenPed Administrator Interface</FONT></FONT><BR>&nbsp;</CENTER>\n",
			TIMES_14,ob_user);
	   close_cell();
	   print_bot();
	   exit(0);
	}

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
		print_top(1,0,strtmp); 
		printf("<FONT COLOR=white>PROBLEM WITH POST[%s]/GET[%s] TRANSMISSION<br>\n",c,c2);
	}


/******************************************************************************/
/* MAIN PAGE	 						  	      */
/******************************************************************************/

            if (count==0)
            {
		print_top(0,0,strtmp); 
		printf("<BR><CENTER>");
		printf("%s<FONT COLOR=white><B>Main Menu</B></FONT></FONT><BR>&nbsp;</CENTER>\n",TIMES_14);
		make_sub_cell();
                do_main_page(0,ob_user);
		close_cell();
            }
            else
/******************************************************************************/
/* GP_PENDING                                                                 */
/******************************************************************************/
	    if (strncmp(entries[0].name,"GP_PENDING",10)==0)
	    {
		print_top(1,0,strtmp);
		printf("<BR><CENTER>");
		printf("%s<FONT COLOR=white><B>List of pending new pedigrees</B></FONT></FONT><BR>&nbsp;</CENTER>\n",TIMES_14);
		make_sub_cell();
		list_pending(ob_user);
		close_cell();
	    }
	    else
/******************************************************************************/
/* GP_EXAMINE                                                                 */
/******************************************************************************/
            if (strncmp(entries[0].name,"GP_EXAMINE",10)==0)
            {
                print_top(1,0,strtmp);
		if ((strncmp(entries[1].name,"TICKET_ID",9)!=0)||
		    (strncmp(entries[2].name,"PED_ID",6)!=0))
			exitpage(INCORRECT_ENTRIES);
		if ((sscanf(entries[1].val,"%ld",&ticket_id)!=1)||
		    (sscanf(entries[2].val,"%d",&ped_id)!=1))
			exitpage(INCORRECT_ENTRIES);
                printf("<BR><CENTER>");
                printf("%s<FONT COLOR=white><B>Information for Pending Pedigree %d</B></FONT></FONT><BR>&nbsp;</CENTER>\n",TIMES_14,ped_id+1);
		make_sub_cell();
                examine_pending(ob_user, ped_id, ticket_id);
		close_cell();
            }
            else
/******************************************************************************/
/* GP_EXAMINE                                                                 */
/******************************************************************************/
            if (strncmp(entries[0].name,"GP_USERS",8)==0)
            {
                print_top(1,0,strtmp);
                printf("<BR><CENTER>");
                printf("%s<FONT COLOR=white><B>User Administration Interface</B></FONT></FONT><BR>&nbsp;</CENTER>\n",TIMES_14);
                make_sub_cell();
                list_users(ob_user);
                close_cell();
            }
            else
/******************************************************************************/
/* GP_QUESTIONS                                                               */
/******************************************************************************/
            if (strncmp(entries[0].name,"GP_QUESTIONS",12)==0)
            {
                print_top(1,0,strtmp);
                printf("<BR><CENTER>");
                printf("%s<FONT COLOR=white><B>Online Questionnaire</B> ",TIMES_14);
                i=show_questions();
		printf("<p><CENTER><A HREF=%s_admin?NEW_QUESTION=%d><FONT COLOR=%s>Add a New Question</FONT></A>",
                DB_NAME, i, bgcol[7]);
            }
            else
/******************************************************************************/
/* REMOVE_QUESTION                                                            */
/******************************************************************************/
            if (strncmp(entries[0].name,"REMOVE_QUESTION",15)==0)
            {
                print_top(1,0,strtmp);
		if (sscanf(entries[0].val,"%d",&i)!=1)
			exitpage(INCORRECT_ENTRIES);
                printf("<BR><CENTER>");
		rem_question_from_file(i);
                printf("%s<FONT COLOR=white><B>Question %d has been removed from questionnaire.</B> Please follow <A HREF=javascript:history.go(-1)>this link</A>.</FONT></FONT></CENTER>\n",TIMES_14,i+1);
            }
            else
/******************************************************************************/
/* ADD_TOKENS                                                                 */
/******************************************************************************/
            if (strncmp(entries[0].name,"ADD_TOKENS",10)==0)
            {
                print_top(1,1,strtmp);
		if ((strncmp(entries[1].name,"QUESTION",8)!=0)||
		    (strncmp(entries[2].name,"MODE",4)!=0)||
		    (strncmp(entries[3].name,"NBTOKENS",8)!=0)||
		    (strncmp(entries[4].name,"DEFAULT",7)!=0))
			exitpage(INCORRECT_ENTRIES);
		q=(QUESTION *)create_question();
		q->question=(char *)malloc(strlen(entries[1].val)*sizeof(char));
		strcpy(q->question,entries[1].val);
		sscanf(entries[2].val,"%d",&i);
		q->mode=i;
		sscanf(entries[3].val,"%d",&i);
		q->value=(char *)malloc(strlen(entries[4].val)*sizeof(char));
		strcpy(q->value,entries[4].val);
		q->nbtokens=i;
		q->tokens=(char **)malloc(i*sizeof(char *));
		j=5;
		for (i=0;i<q->nbtokens;i++)
		{
		  q->tokens[i]=(char *)
				malloc(strlen(entries[j].val)*sizeof(char));
		  strcpy(q->tokens[i],entries[j++].val);
		}
		add_question_to_file(q);
		free(q);
                printf("<CENTER><BR>%s<FONT COLOR=white><B>Your question has been added to the Online Questionnaire.</B> Please follow <A HREF=javascript:history.go(-3)>this link</A> now.</FONT></FONT></CENTER>\n",TIMES_14);
            }
            else
/******************************************************************************/
/* NEW_QUESTION                                                               */
/******************************************************************************/
	    if (strncmp(entries[0].name,"NEW_QUESTION",12)==0)
            {
                print_top(1,1,strtmp);
		if (sscanf(entries[0].val,"%d",&i)==1)
		{
                  printf("<BR><CENTER>");
                  printf("%s<FONT COLOR=white><B>Adding Question %d to the Questionnaire Form</B></FONT></FONT><BR>&nbsp;</CENTER>\n",TIMES_14,i+1);
		  make_sub_cell();
		  show_new_question(i);
		  close_cell();
		}
		else
		{
			printf("<BR>Invalid Entry...\n");
		}
            }
            else
/******************************************************************************/
/* ADD_QUESTION                                                               */
/******************************************************************************/
            if (strncmp(entries[0].name,"ADD_QUESTION",12)==0)
            {
                print_top(1,1,strtmp);
                if (sscanf(entries[0].val,"%d",&i)!=1)
			exitpage(INCORRECT_ENTRIES);
		if (strncmp(entries[1].name,"QUESTION",8)!=0)
			exitpage(INCORRECT_ENTRIES);
		if (strlen(entries[1].val)<2)
		{
		   printf("<CENTER><BR>%sYou have not formulated your Question in the previous form. (<A HREF=javascript:history.go(-1)>Go Back</A>)<BR>",TIMES_12);
		   print_bot();
		   exit(0);
		}
		q=(QUESTION *)create_question();
		q->id=i;
		q->question=(char *)malloc(strlen(entries[1].val)*sizeof(char));
		strcpy(q->question,entries[1].val);
                printf("<BR><CENTER>");
                printf("%s<FONT COLOR=white><B>Adding Question %d to the Questionnaire Form</B></FONT></FONT><BR>&nbsp;</CENTER>\n",TIMES_14,i+1);
                make_sub_cell();
		i=1;	/* Wait for the MODE entry */
		while (strncmp(entries[i].name,"MODE",4)!=0) i++;
		sscanf(entries[i].val,"%d",&j);
		q->mode=j;
		i++;
		switch(q->mode)
		{
		   case QMODE_YESNO:
			if (strncmp(entries[i].name,"DEFAULT",7)!=0)
				exitpage(INCORRECT_ENTRIES);
			sscanf(entries[i].val,"%d",&j);
			q->value=(char *)
				malloc(strlen(entries[i].val)*sizeof(char));
			strcpy(q->value,entries[i].val);
			q->nbtokens=2;
			q->tokens=(char **)malloc(2*sizeof(char *));
			q->tokens[0]=
				(char *)malloc(strlen(NO_STR)*sizeof(char));
			strcpy(q->tokens[0],NO_STR);
			q->tokens[1]=
				(char *)malloc(strlen(YES_STR)*sizeof(char));
			strcpy(q->tokens[1],YES_STR);
			add_question_to_file((QUESTION *)q);
			break;
		   case QMODE_RADIO:
		   case QMODE_MENU:
		   case QMODE_LIST:
		   case QMODE_MULTI:
			if (strncmp(entries[i].name,"NBTOKENS",8)!=0)
                                exitpage(INCORRECT_ENTRIES);
                        sscanf(entries[i].val,"%d",&j);
                        q->nbtokens=j;
                        q->tokens=(char **)NULL;
                        i++;
                        if (strncmp(entries[i].name,"DEFAULT",7)!=0)
                                exitpage(INCORRECT_ENTRIES);
                        q->value=(char *)
                                malloc(strlen(entries[i].val)*sizeof(char));
                        strcpy(q->value,entries[i].val);
                        ask_tokens(q);
			no_chears=1;
			break;
		   case QMODE_VALUE:
			if (strncmp(entries[i].name,"NBTOKENS",8)!=0)
                                exitpage(INCORRECT_ENTRIES);
                        sscanf(entries[i].val,"%d",&j);
                        q->nbtokens=1;
                        q->tokens=(char **)malloc(sizeof(char *));
                        q->tokens[0]=(char *)
                                malloc(strlen(entries[i].val)*sizeof(char));
                        strcpy(q->tokens[0],entries[i].val);
                        i++;
                        if (strncmp(entries[i].name,"DEFAULT",7)!=0)
                                exitpage(INCORRECT_ENTRIES);
                        sscanf(entries[i].val,"%d",&j);
                        q->value=(char *)
                                malloc(strlen(entries[i].val)*sizeof(char));
                        strcpy(q->value,entries[i].val);
                        add_question_to_file((QUESTION *)q);
			break;
		   case QMODE_TEXTAREA:
			if (strncmp(entries[i].name,"NBTOKENS",8)!=0)
                                exitpage(INCORRECT_ENTRIES);
                        sscanf(entries[i].val,"%d",&j);
                        q->nbtokens=1;
                        q->tokens=(char **)malloc(sizeof(char *));
                        q->tokens[0]=(char *)
				malloc(strlen(entries[i].val)*sizeof(char));
                        strcpy(q->tokens[0],entries[i].val);
			i++;
			if (strncmp(entries[i].name,"DEFAULT",7)!=0)
                                exitpage(INCORRECT_ENTRIES);
                        sscanf(entries[i].val,"%d",&j);
                        q->value=(char *)
                                malloc(strlen(entries[i].val)*sizeof(char));
                        strcpy(q->value,entries[i].val);
                        add_question_to_file((QUESTION *)q);
                        break;
		}
                if (!no_chears) printf("<CENTER><BR>%s<FONT COLOR=white><B>Your question has been added to the Online Questionnaire.</B> Please follow <A HREF=javascript:history.go(-2)>this link</A> now.</FONT></FONT></CENTER>\n",TIMES_14);
		free_question(q);
                close_cell();
            }
            else
/******************************************************************************/
/* UNKNOWN OPTION                                                             */
/******************************************************************************/
    	    {
		print_top(1,1,strtmp);
                printf("<BR><BR>");
		make_sub_cell();
      		printf("<BR><CENTER><FONT COLOR=white>Unknown Option or Under Development</FONT><BR>&nbsp;</CENTER>\n");
		close_cell();
	    }
	    print_bot();
}
