#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gpconfig.h"
#include "gpadmin.h"
#include "gphtml.h"

#define MAX_USERS	255

int is_admin_user(char *user)
{
 FILE *fd=NULL;
 FILE *fdd=NULL;
 char line[255]={""};
 while ((fdd=fopen(DB_USERADMIN_LOCK,"r"))!=NULL)
 {
	fclose(fdd);
	sleep(1);
 }
 fdd=fopen(DB_USERADMIN_LOCK,"w");
 fclose(fdd);
 fd=fopen(DB_USERADMIN_FILE,"r");
 while (!feof(fd))
 {
    fgets(line,255,fd);
    if (!feof(fd))
    {
	line[strlen(line)-1]='\0';
	if (strcmp(line,user)==0)
	{
	   fclose(fd);
	   remove(DB_USERADMIN_LOCK);
	   return 1;
	}
    }
 }
 fclose(fd);
 remove(DB_USERADMIN_LOCK);
 return 0;
}

void list_users(char *ob_user)
{
 time_t Sometime;
 int nbpending=0;
 FILE *fd=NULL;
 char line[255]={""};
 char strtmp[255]={""};
 int nbinlist=0;
 char list[MAX_USERS][30];	/* MAX_USERS */
 long tstamp[MAX_USERS];	/* MAX_USERS */
 char already=0;
 int i=0;
 FILE *fdd=NULL;
 Sometime=time(&Sometime);
  while ((fdd=fopen(DB_USERAUTH_LOCK,"r"))!=NULL)
  {
	fclose(fdd);
	sleep(1);
  }
  fdd=fopen(DB_USERAUTH_LOCK,"w");
  fclose(fdd);
  fd=fopen(DB_USERAUTH_FILE,"r");
  while (!feof(fd))
  {
    fgets(line,255,fd);
    if (!feof(fd))
    {
	line[strlen(line)-1]='\0';
	if (sscanf(line,"%s %ld",list[nbinlist], &(tstamp[nbinlist]))==2)
		nbinlist++;
	
    }
  }
  fclose(fd);
  remove(DB_USERAUTH_LOCK);
  if (nbinlist==0)
  {
    printf("%sNo user can access the %s GenPed Public Database...</FONT><p>",TIMES_12,DB_HTML_NAME);
    return;
  }
  if (nbinlist==1)
    printf("%sOne user can access the %s GenPed Public Database...</FONT><p>",TIMES_12,DB_HTML_NAME);
  else
    printf("%s%d users can access the %s GenPed Public Database...</FONT><p>",
		TIMES_12,nbinlist,DB_HTML_NAME);
  printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
  printf("<TH BGCOLOR=%s WIDTH=20%%>%s<FONT COLOR=black>Username</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=30%%>%s<FONT COLOR=black>Date Created</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=15%%>%s<FONT COLOR=black>Admin</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=20%%>%s<FONT COLOR=black>&nbsp;</FONT></FONT></TH>\n",bgcol[7],TIMES_10);
  printf("<TH BGCOLOR=%s WIDTH=15%%>%s<FONT COLOR=black>&nbsp;</FONT></FONT></TH>\n",bgcol[7],TIMES_10);
  printf("</TR>");
#if 1
  for (i=0;i<nbinlist; i++)
  {
    printf("<FORM METHOD=GET ACTION=%s_admin>",DB_NAME);
    printf("<INPUT TYPE=HIDDEN NAME=UPDATE_USER VALUE=1></INPUT>\n");
    printf("<TR>\n");
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>%s</FONT></FONT></TH>\n",TIMES_12,list[i]);
    Sometime=tstamp[i];
    strcpy(strtmp,ctime(&Sometime));
    strtmp[strlen(strtmp)-1]='\0';
  printf("<TH BGCOLOR=white WIDTH=30%%>%s<FONT COLOR=black>%s</FONT></FONT></TH>\n",TIMES_12,strtmp);
    if (is_admin_user(list[i]))
  	printf("<TH BGCOLOR=white WIDTH=15%%>%s<FONT COLOR=black><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white><SELECT TYPE=MENU NAME=EDITOR><OPTION VALUE='Yes' SELECTED>Yes</OPTION><OPTION VALUE='No'>No</OPTION></SELECT></FONT></TH></TR></TABLE></FONT></FONT></TH>\n",TIMES_12);
    else
  	printf("<TH BGCOLOR=white WIDTH=15%%>%s<FONT COLOR=black><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white><SELECT TYPE=MENU NAME=EDITOR><OPTION VALUE='No' SELECTED>No</OPTION><OPTION VALUE='Yes'>Yes</OPTION></SELECT></FONT></TH></TR></TABLE></FONT></FONT></TH>\n",TIMES_12);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<A HREF=%s_admin?GP_CHANGE_PASSWORD=1&USER='%s'><FONT COLOR=blue>Change Password</FONT></A></FONT></TH>\n",TIMES_10,DB_NAME,list[i]);
  printf("<TH BGCOLOR=white WIDTH=15%%>%s<A HREF=%s_admin?GP_UPDATE_USER=1&USER='%s'><FONT COLOR=blue>Update User</FONT></A></FONT></TH>\n",TIMES_10,DB_NAME,list[i]);
     printf("</TR>\n");
     printf("</FORM>");
  }
#endif
  printf("</TABLE>");
}

