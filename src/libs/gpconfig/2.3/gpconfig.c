#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define DEBUG	0

/* This file provides all global variables to the program */

#include "vars.h"

int setup_global(int , char *);

static char stop_filename[255]={""};
static char db_stop_filename[255]={""};
static FILE *fd=NULL;

char gpconffile[255]={""};

/* Number of existing tokens to parse from gp.conf file */

#define GP_CONF_MAX_PARAMS	17

/* THE _PROG tokens need to be at the end !! */
static char gpparams[GP_CONF_MAX_PARAMS][255]={
/*0*/	"GP_UID",			/* Lexdb pseudo user UNIX UID */
/*1*/	"GP_GID", 			/* Lexdb pseudo user's UNIX GID */
/*2*/	"GP_LOCAL_DOMAIN",		/* Local domain */
/*3*/	"DB_OWNER",			/* A database owner (gpadmin) */
/*4*/	"DB_HTML_NAME",			/* Minimal size description/Name */
/*5*/	"DB_EXTENDED_NAME",		/* Detailled description <P ALIGN> */
/*6*/	"DB_PREFIX",			/* Usually PEDID but that can't change */
/*7*/	"DB_LOGOGIF_FILE",		/* Logo GIF file (main page) */
/*8*/	"DB_LOGOGIF_WIDTH",		/* Logo width */
/*9*/	"DB_LOGOGIF_HEIGHT",		/* Logo Height */
/*10*/	"DB_ICONGIF_FILE",              /* Icon GIF file (menu frame) */
/*11*/	"DB_ICONGIF_WIDTH",             /* Icon width */
/*12*/	"DB_ICONGIF_HEIGHT",             /* Icon Height */
/*13*/  "DB_DISORDER_NAME",		/* Some name to appear */
/*14*/  "GP_HTDOCS_DIR",			/* rootdir for htdocs */
/*15*/  "DB_FONT_COLOR",			/* Project font color */
/*16*/  "GP_MAX_GENERATIONS"		/* REFERENCE OF PARENTS AT THE TOP */

};

/* Returns 0 if not or 1 if ok */
int check_if_db_exists(char *dbname)
{
 FILE *f=NULL;
 char fname[255]={""};
 char line[1024]={""};
 char chkstr[255]={""};
 char found=0;
 sprintf(fname,"%s/data/%s/etc/%s.conf", GP_ROOT_DIR, dbname, dbname);
 if ((f=fopen(fname,"r"))==NULL)
 {
	return 0;
 }
 fclose(f);
 return 1;
}

/* This function is used to parse the gp.conf file only */
static int get_gptoken(char *token)
{
  int i;
  for (i=0;i<GP_CONF_MAX_PARAMS;i++)
  {
	if (strcmp(token,gpparams[i])==0)
	   return i;
  }
  return -1;	/* Can't find token -> error */
}

int get_environments()
{
  if (getenv("GP_ROOT_DIR")!=NULL)
          strcpy(GP_ROOT_DIR,getenv("GP_ROOT_DIR"));
  else
	  strcpy(GP_ROOT_DIR,"/home/genped");
  if (chdir(GP_ROOT_DIR)<0)
	return 0;
  
  if (getenv("REMOTE_USER")!=NULL)
          strcpy(DB_USER,getenv("REMOTE_USER"));
  else
	strcpy(DB_USER,"genped");

  return 1;
}

/* This function sets all the global variables for a database */
/* 1 returned on success, 0 on error */
int gp_get_config(char *dbname)
{
 FILE *fd=NULL;
 int i=0;
 int pos=0, pos2=0, pos3=0;
 char gpconffile[255]={""};
 char line[1024]={""};
 char line2[1024]={""};
 char line3[255]={""};
 int k=0;

  /* We get the environment variables that should be already set */
  if (get_environments()==0)
  {
		printf("Problem loading environment.\n");
		return 0;
  }
  if(check_if_db_exists(dbname)<1)
  {	
		printf("[%s]: No such GenPed Collection Available.\n", dbname);
		exit(0);
  }
  strcpy(DB_NAME,dbname);
  for (k=0; k<2; k++)
  {
     switch(k)
     {
	case 0: sprintf(gpconffile,"%s/etc/genped.conf", GP_ROOT_DIR);
		break;
     	case 1:	sprintf(gpconffile,"%s/data/%s/etc/%s.conf",
						GP_ROOT_DIR, DB_NAME, DB_NAME);
		break;
     }
  /* First we load gp.conf, then db.conf */

     if ((fd=fopen(gpconffile,"r"))==NULL)
     {
	perror(gpconffile);
	exit(0);
     }
#if DEBUG
     printf("parsing %s ...\n",gpconffile);
#endif
     while (!feof(fd))
     {
	if (fgets(line,1024,fd)!=NULL)
	{
	  if ((line[0]==9)||(line[0]==' '))
		strcpy(line,line+1);
	  if ((line[0]!='#')&&(strchr(line,'=')!=NULL))	/* If no equal not even bother */
	  {
	    line[strlen(line)-1]='\0';
	    strcpy(line2,""); pos=0; pos2=0;
			/* Until we got an equal sign */
	    while ((pos<strlen(line))&&(line[pos]!='='))
	    {
		if ((line[pos]!=' ')&&(line[pos]!=9))	/* Clean up TAB & SPC */
			line2[pos2++]=line[pos];
		pos++;
	    }
	    line2[pos2]='\0';	/* We should have a token here */
	    strcpy(line3,line+pos+2);
	    while (line3[pos3]==' ') pos3++;
	    strcpy(line3,line3+pos3);
	    if((i=get_gptoken(line2))== -1)
	    {
		printf("Unknown token [%s]\n",line2);
	    }
	    setup_global(i,line3);
          }
        }
     }
     fclose(fd);
  }
  return 1;
}

int setup_global(int i, char *l)
{
char line[255]={""};
char strtmp[255]={""};
int returncode=1;
	    strcpy(line,l);
	    switch(i)	/* GLOBALS SETTINGS */
	    {
		case 0:
		    	if (sscanf(line,"%d",&GP_UID)!=1)
		    	{
			   printf("Error parsing GP_UID.\n");
			   returncode=-1;
		    	}
		    	break;
		case 1:
			if (sscanf(line,"%d",&GP_GID)!=1)
                        {
                           printf("Error parsing GP_GID.\n");
                           returncode=-1;
                        }   
                        break;
		case 2:
			if (strcpy(GP_LOCAL_DOMAIN,line)==NULL)
			{
                           printf("Error parsing GP_LOCAL_DOMAIN.\n");
                           returncode=-1;
                        }   
                        break;
		case 3:
			if (strcpy(DB_OWNER,line)==NULL)
                        {
                           printf("Error parsing DB_OWNER.\n");
                           returncode=-1;
                        }   
                        break;
		case 4:
                         if (strcpy(DB_HTML_NAME,line)==NULL)
                        {
                           printf("Error parsing DB_HTML_NAME.\n");
                           returncode=-1;
                        }   
                        break;
		case 5:
                        if (strcpy(DB_EXTENDED_NAME,line)==NULL)
                        {
                           printf("Error parsing DB_EXTENDED_NAME.\n");
			   returncode=-1;
                        }   
                        break;
		case 6:
			if (strcpy(DB_PREFIX,line)==NULL)
                        {
                           printf("Error parsing DB_PREFIX.\n");
			   returncode=-1;
                        }  
                        break;
		case 7:
			if (strcpy(DB_LOGOGIF_FILE,line)==NULL)
			{
			   printf("Error parsing DB_LOGOGIF_FILE.\n");
			   returncode=-1;
			}
			break;
		case 8:
			if (sscanf(line,"%d",&DB_LOGOGIF_WIDTH)!=1)
			{
			   printf("Error parsing DB_LOGOGIF_WIDTH.\n");
			   returncode=-1;
			}
			break;
		case 9:
			if (sscanf(line,"%d",&DB_LOGOGIF_HEIGHT)!=1)
			{
			   printf("Error parsing DB_LOGOGIF_HEIGHT.\n");
			   returncode=-1;
			}
			break;
		case 10:
                        if (strcpy(DB_ICONGIF_FILE,line)==NULL)
                        {
                           printf("Error parsing DB_ICONGIF_FILE.\n");
                           returncode=-1;
                        }
                        break;
                case 11:
                        if (sscanf(line,"%d",&DB_ICONGIF_WIDTH)!=1)
                        {
                           printf("Error parsing DB_ICONGIF_WIDTH.\n");
                           returncode=-1;
                        }
                        break;
                case 12:
                        if (sscanf(line,"%d",&DB_ICONGIF_HEIGHT)!=1)
                        {
                           printf("Error parsing DB_ICONGIF_HEIGHT.\n");
                           returncode=-1;
                        }
                        break;
		case 13:
			if (strcpy(DB_DISORDER_NAME,line)==NULL)
                        {
                           printf("Error parsing DB_DISORDER_NAME.\n");
                           returncode=-1;
                        }
                        break;
		case 14:
			if (strcpy(GP_HTDOCS_DIR,line)==NULL)
                        {  
                           printf("Error parsing GP_HTDOCS_DIR.\n");
                           returncode=-1;
                        }
                        break;
		case 15:
                        if (strcpy(DB_FONT_COLOR,line)==NULL)
                        {
                           printf("Error parsing DB_FONT_COLOR.\n");
                           returncode=-1;
                        }
                        break;
		case 16:
                        if (sscanf(line,"%d",&GP_MAX_GENERATIONS)!=1)
                        {
                           printf("Error parsing GP_MAX_GENERATIONS.\n");
                           returncode=-1;
                        }
			if (GP_MAX_GENERATIONS>5)
				GP_MAX_GENERATIONS=5;
			if (GP_MAX_GENERATIONS<0)
				GP_MAX_GENERATIONS=0;
                        break;
	}
	return returncode;
}

