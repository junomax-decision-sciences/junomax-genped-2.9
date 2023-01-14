#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gpconfig.h"
#include "gpadmin.h"
#include "gphtml.h"


void list_pending(char *ob_user)
{
 time_t Sometime;
 int nbpending=0;
 FILE *fd=NULL;
 char line[255]={""};
 char strtmp[255]={""};
 int nbinlist=0;
 long *list=NULL;
 char already=0;
 int i=0;
 Sometime=time(&Sometime);
  fd=fopen(DB_PED_SPOOL_FILE,"r");
  while (!feof(fd))
  {
    fgets(line,255,fd);
    if (!feof(fd))
    {
	line[strlen(line)-1]='\0';
	if (sscanf(line,"%ld",&Sometime)==1)
		nbinlist++;
	
    }
  }
  fclose(fd);
  if (nbinlist==0)
  {
    printf("%sThere are no pending Pedigree to process at this time...</FONT><p>",TIMES_10);
    return;
  }
  if (nbinlist==1)
    printf("%sThere is 1 pending Pedigree to process...</FONT><p>",TIMES_10);
  else
    printf("%sThere are %d pending Pedigrees to process...</FONT><p>",
		TIMES_10,nbinlist);
  list=(long *)malloc(nbinlist*sizeof(long));
  nbinlist=0;
  fd=fopen(DB_PED_SPOOL_FILE,"r");
  while (!feof(fd))
  {
    fgets(line,255,fd);
    if (!feof(fd))
    {
        line[strlen(line)-1]='\0';
        if (sscanf(line,"%ld",&(list[nbinlist]))==1)
                nbinlist++;
     
    }
  }
  fclose(fd);
  printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
  printf("<TH BGCOLOR=%s WIDTH=10%%>%s<FONT COLOR=black>Pedigree</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=20%%>%s<FONT COLOR=black>Ticket Number</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=50%%>%s<FONT COLOR=black>Date Entered</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=20%%>%s<FONT COLOR=black>&nbsp;</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("</TR>");
  for (i=0;i<nbinlist; i++)
  {
     printf("<TR>\n");
  printf("<TH BGCOLOR=white WIDTH=10%%>%s<FONT COLOR=black>%ld</FONT></FONT></TH>\n",TIMES_12,i+1);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>%ld</FONT></FONT></TH>\n",TIMES_12,list[i]);
     Sometime=list[i];
     strcpy(strtmp,ctime(&Sometime));
     strtmp[strlen(strtmp)-1]='\0';
  printf("<TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>%s</FONT></FONT></TH>\n",TIMES_12,strtmp);
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<A HREF=%s_admin?GP_EXAMINE=1&TICKET_ID=%ld&PED_ID=%d><FONT COLOR=blue>Examine</FONT></A></FONT></TH>\n",TIMES_12,DB_NAME,list[i],i);
     printf("</TR>\n");
  }
  printf("</TABLE>");
}

void examine_pending(char *ob_user, int ped_id, long ticket_id)
{
 char strtmp[255]={""};
  printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
  printf("<TH BGCOLOR=%s WIDTH=20%%>%s<FONT COLOR=black>Pedigree</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=30%%>%s<FONT COLOR=black>Ticket Number</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("<TH BGCOLOR=%s WIDTH=50%%>%s<FONT COLOR=black>Date Entered</FONT></FONT></TH>\n",bgcol[7],TIMES_12);
  printf("</TR>");
  printf("<TR>\n");
  printf("<TH BGCOLOR=white WIDTH=20%%>%s<FONT COLOR=black>%ld</FONT></FONT></TH>\n",TIMES_12,ped_id+1);
  printf("<TH BGCOLOR=white WIDTH=30%%>%s<FONT COLOR=black>%ld</FONT></FONT></TH>\n",TIMES_12,ticket_id);
     strcpy(strtmp,ctime(&ticket_id));
     strtmp[strlen(strtmp)-1]='\0';
  printf("<TH BGCOLOR=white WIDTH=50%%>%s<FONT COLOR=black>%s</FONT></FONT></TH>\n",TIMES_12,strtmp); 
  printf("</TR>\n");
  printf("</TABLE>");
}
