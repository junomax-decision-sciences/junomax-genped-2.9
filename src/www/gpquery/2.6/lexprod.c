/* This program does only one thing; add an annotation to the database */

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

#include "sybfront.h"
#include "sybdb.h"

#include "lexconfig.h"
#include "dbproc.h"
#include "dbpfam.h"
#include "dbtissue.h"
#include "util.h"
#include "lexhtml.h"
#include "multiform.h"
#include "lexexec.h"

#if 1
#include "pubdbconfig.h"
#include "dataset_util.h"
#endif


extern char PUBDB_ROOT_DIR[255];

#include "align.h"	/* Manuel's library */

#define VERBOSE 1
#define OUTPUT_QUERY	0

static char seqstr[64]={"pn"};

static struct stat statfile;

void lexgenLog (char *m)
{
printf("\n%s\n",m);
}

char pfn[255]={""};

int print_pfamilies_table(DBPROCESS *dbproc,char *searchquery)
{
char already=0;
int i=0;
RETCODE rt;
PFAM_INFO *ppf=NULL;
		if (strlen(searchquery)>0)
		{
        	   	dbfcmd(dbproc,"select distinct %s_hits_table.name,PFAM_Info_Table.description, PFAM_Info_Table.avlength, PFAM_Info_Table.avid, PFAM_Info_Table.structure, PFAM_Info_Table.seed, PFAM_Info_Table.full ", DB_NAME);
			
			dbfcmd(dbproc,"from %s_hits_table, PFAM_Info_Table where %s_hits_table.name like '%s' and PFAM_Info_Table.name=%s_hits_table.name order by %s_hits_table.name",
				DB_NAME, DB_NAME,
				searchquery,
				DB_NAME, DB_NAME);
		}
		else
        	   dbfcmd(dbproc,"select distinct name from %s_hits_table order by name",DB_NAME);
        	dbsqlexec(dbproc);
		i=0;
        	while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS)
        	{
                  if (rt==SUCCEED)
                  {
/*
   Name         acc    #seed  #full  av.   av.  structure        Description
                   number                len   %id */

		    ppf=(PFAM_INFO *)malloc_pfam();

                    dbbind(dbproc,1,STRINGBIND,(DBINT)0,(BYTE DBFAR *)(ppf->name));
                    dbbind(dbproc,2,STRINGBIND,(DBINT)0,(BYTE DBFAR *)(ppf->description));
                    dbbind(dbproc,3,SMALLBIND,(DBINT)0,(BYTE *)&(ppf->avlength));
                    dbbind(dbproc,4,TINYBIND,(DBINT)0,(BYTE *)&(ppf->avid));
                    dbbind(dbproc,5,STRINGBIND,(DBINT)0,(BYTE DBFAR *)(ppf->structure));
                    dbbind(dbproc,6,SMALLBIND,(DBINT)0,(BYTE *)&(ppf->seed));
                    dbbind(dbproc,7,SMALLBIND,(DBINT)0,(BYTE *)&(ppf->full));
                    while (dbnextrow(dbproc)!=NO_MORE_ROWS)
                    {
                        if (!already)
                        {
                          already=1;
			  make_sub_cell("white","white","#209090");
                          printf("<TABLE BORDER=1 BGCOLOR=black WIDTH=100%%><TR>");
                    	  printf("<TR><TD BGCOLOR=#209090>%s<A HREF=http://pfam.wustl.edu TARGET=PFAM_WIN><FONT COLOR=black><B>PFAM Name</B></FONT></A></FONT></TD>",TIMES_10);
			  printf("<TD BGCOLOR=#209090>%s<FONT COLOR=black>Protein Family Description</FONT></FONT></TD>",TIMES_10);
			  printf("<TD BGCOLOR=#209090>%s<FONT COLOR=black>Av.Length</FONT></FONT></TD>",TIMES_10, ppf->avlength);
			  printf("<TD BGCOLOR=#209090>%s<FONT COLOR=black>Ident.</FONT></FONT></TD>",TIMES_10, ppf->avid);
			  printf("<TD BGCOLOR=#209090>%s<FONT COLOR=black>Structure</FONT></FONT></TD>",TIMES_10);
			  printf("<TD BGCOLOR=#209090>%s<FONT COLOR=black>#Seed</FONT></FONT></TD>",TIMES_10);
			  printf("<TD BGCOLOR=#209090>%s<FONT COLOR=black>#Full</FONT></FONT></TD></TR>",TIMES_10);
                    	}
				printf("<TR><TD BGCOLOR=white>%s<A HREF=http://pfam.wustl.edu/cgi-bin/getdesc?name=%s TARGET=PFAM_WIN><FONT COLOR=blue><B>%s</B></FONT></A></FONT> ",TIMES_10,ppf->name,ppf->name);
#if 0
				printf("(<A HREF=%s?PFAM_FAMILY_HITS=%s TARGET=MAINFRAME>%s<FONT COLOR=blue>Hits</FONT></FONT></A>)</FONT></TD>",DB_NAME,ppf->name,TIMES_8);
#endif
			printf("<TH BGCOLOR=white><A HREF=\"javascript://\" onClick=\"window.open('%s?PFAM_FAMILY_HITS=%s','PFAM_FAMILIES', 'scrollbars=yes,resizable=yes,width=600,height=400,top=10,left=10');\">%s<FONT COLOR=#209090>[Hits]</FONT></FONT></A></TH>",DB_NAME,ppf->name,TIMES_8);
			printf("<TD BGCOLOR=white>%s<FONT COLOR=black>%s</FONT></FONT></TD>",TIMES_10,ppf->description);
			printf("<TD BGCOLOR=white>%s<FONT COLOR=black>%d</FONT></FONT></TD>",TIMES_10, ppf->avlength);
			printf("<TD BGCOLOR=white>%s<FONT COLOR=black>%d%%</FONT></FONT></TD>",TIMES_10, ppf->avid);
			if (strlen(ppf->structure)>1)
				printf("<TD BGCOLOR=white><A HREF=http://www.ncbi.nlm.nih.gov/cgi-bin/Structure/mmdbsrv?uid=%s&form=6&db=t&Dopt=s TARGET=EXTERNDB>%s<FONT COLOR=blue>%s</FONT></FONT> <SUP><A HREF=http://www.ncbi.nlm.nih.gov/cgi-bin/Structure/mmdbsrv?uid=%s&form=6&db=t&save=&dopt=r&Complexity=Cn3D+Subset&KinemageRender=63>%s<FONT COLOR=black>Ras</FONT></FONT></A></SUP></TD>",ppf->structure,TIMES_10, ppf->structure,ppf->structure,TIMES_8);
			else
				printf("<TD BGCOLOR=white>%s<FONT COLOR=black>&nbsp;</FONT></FONT></TD>",TIMES_10);
			printf("<TD BGCOLOR=white>%s<FONT COLOR=black>%d</FONT></FONT></TD>",TIMES_10,ppf->seed);
			printf("<TD BGCOLOR=white>%s<FONT COLOR=black>%d</FONT></FONT></TD></TR>",TIMES_10, ppf->full);
			i++;
               	    }
	    	  }
		}
		if (already)
		{
		   printf("</TABLE>");
		   close_cell();
		}
		else
		{
		   printf("%s<FONT COLOR=white>No Protein Family with name starting with [%c] and hit(s) in the %s Database was found</FONT></FONT>",TIMES_10,searchquery,DB_HTML_NAME);
		}
	free(ppf);
	return (int)already;
}
/* Prints out a message from LEXDB_ROOT_DIR/databases/DB_NAME/etc/messages */
/* Each line in this file will include an id and a filename in the form
   message.id should exists in this same directory (or a link) */
/* if an empty file exists in the ASAP users dir for this user, then the 
   message has already been read and hence should not be displayed */
void check_messages()
{
FILE *fd=NULL;
FILE *fd2=NULL;
char fname[255]={""};
char strtmp[255]={""};
char strtmp2[255]={""};
long id=0;	/* The id is the timestamp of the date of the message (mktime)*/
int i,j;
char line[255]={""};
   chdir("/tmp");
   sprintf(fname,"%s/databases/%s/etc/messages", LEXDB_ROOT_DIR, DB_NAME);
   if ((fd=fopen(fname,"r"))==NULL) return;	/* No messages */
   while (!feof(fd))
   {
 	if (fgets(line,255,fd)!=NULL)
	{
	  line[strlen(line)-1]='\0';	/* Remove trailing \n */
	  if (sscanf(line,"%ld",&id)==1)
	  {
   		sprintf(strtmp,"%s/databases/%s/users", LEXDB_ROOT_DIR, DB_NAME);
		if (chdir(strtmp)<0)
		{
		   fclose(fd);
		   return;
		}
   		sprintf(strtmp,"%s/databases/%s/users/%c", LEXDB_ROOT_DIR, DB_NAME,tolower(DB_USER[0]));
		if (chdir(strtmp)<0)
		{
		   mkdir(strtmp,02770);
		   chmod(strtmp,02770);
		}
   		sprintf(strtmp,"%s/databases/%s/users/%c/%s", LEXDB_ROOT_DIR, DB_NAME,tolower(DB_USER[0]),DB_USER);
		if (chdir(strtmp)<0)
                {
                     mkdir(strtmp,02770);
                     chmod(strtmp,02770);
		}
		
   		sprintf(strtmp,"%s/databases/%s/users/%c/%s/message.%ld", LEXDB_ROOT_DIR, DB_NAME,tolower(DB_USER[0]),DB_USER,id);
		if ((fd2=fopen(strtmp,"r"))==NULL)
		{
   		   sprintf(strtmp2,"%s/databases/%s/etc/message.%ld", LEXDB_ROOT_DIR, DB_NAME,id);
		   if ((fd2=fopen(strtmp2,"r"))!=NULL)
		   {
	    	     print_cleartop(1,0,1,"ASAP User Notification","white","black");
		     make_sub_cell("white","white","#209090");
   		     printf("<TABLE BORDER=0 WIDTH=100%%><TR><TD BGCOLOR=white WIDTH=100%% ALIGN=LEFT>");
   		     printf("%s<FONT COLOR=black><BR><BR><BR>",TIMES_12);
		     while (!feof(fd2))
		     {
			if (fgets(line,255,fd2)!=NULL)
			{
			   printf("%s",line);
			}
		     }
		     fclose(fd2);
   		     printf("<BR><BR></FONT></FONT></TD></TR></TABLE>");
		     close_cell();
   		     printf("<P><CENTER><HR>Please <A HREF=%s?NEWS_READ=%ld TARGET=_top><FONT COLOR=#209090>follow this link now</FONT></A> <I>(and the current message will not be displayed next time you access this page).</I></CENTER><P>",DB_NAME,id);
		     printf("</BODY></HTML>");
		     fclose(fd);
		     exit(0);
		   }
		}
		else
			fclose(fd2);	/* Already read */
	  }
	}
   }
   fclose(fd);
}


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


void setLexgenLog(char *m)
{
}

typedef struct {
    char name[8096];
    char val[8096];
} entry;
entry entries[255];


static char save_answer[16000]={""};

#define UPLOAD_DIR	"/tmp/uploads"

#define MAX_OPTIONS	57
static char pairoptions[MAX_OPTIONS+1][255]={
	"MAIN_PAGE",
	"SHOW_SEQUENCE",
	"SHOW_CLUSTER",
	"SEARCH_KEYWORD",
	"SEARCH_SUBSTANCE",
	"Click to view the Database Contents",
	"Click to go to the Advanced Searches Page",
	"CLUSTER_LIST_PAGE",
	"Click to go to the related PubMed Substance Names List Options Page",
	"SHOW_LEXGENES",

	"NEWCLUSTER_LIST_PAGE",
	"Click to report any problem to Technical-Support",
	"SHOW_GIF_BLAST",
	"Click To View The Previous Cluster",
	"Click To View The Next Cluster",
	"DISPLAY_ABI",
	"Click to Search HTST Hits to Protein Families",
	"LIST_PFAM_ORDER_BY_NAME",
	"PFAM_FAMILY_HITS",
	"SHOW_ALIGNM",

	"SHOW_BLASTOFF",
	"INVALID_SEQUENCE",
	"CONFIRM_INVALID_SEQUENCE",
	"DISPLAY_PLATE_DIAGRAM",
	"SHOW_BLAST_ALIGN",
	"VIEW_BLAST_FRAME",
	"SHOW_BLAST_HITS",
	"SHOW_STATS",
	"SHOW_BLAST_RUN",
	"SHOW_BLAST_MULTIALIGN_GIF",

	"SHOW_GIF_BYSTR",
	"SHOW_BLAST_MULTIALIGN",
	"CLUSTER_LIST",
	"SHOW_FASTA_MULTIALIGN",
	"SHOW_TISSUE",
	"PLATE_READY",
	"SHOW_GSP_PAGE",
	"Click to access the ASAP Production options",
	"NEW_PROTOCOL",
	"NEW_PLATE",

	"CREATE_PROTOCOL",
	"LIST_PROTOCOLS",
	"CREATE_PLATE",
	"LIST_PLATES",
	"SHOW_PROTOCOL",
	"SHOW_PLATE_DIAGRAM_BY_NAME",
	"NEW_CLUSTER_LIST",
	"ACTIVETOP",
	"ACTIVEBOT",
	"HELPWIN",
	
	"ONLINEHELP",
	"NEWS_READ",
	"REBLAST_CLUSTER",
	"REBLAST_SUBSETS",
	"PFAM_SEARCH_PAGE",
	"SHOW_PFAM_ALIGN",
	"FIND_ACCESSION"
};


print_usage(char *progname)
{
	printf("Usage: %s DB_NAME STID [append_to_filename]\n",progname);
	printf(" Output to stdout or Append to a FASTA FILE for all valid STs.\n");
	printf("If Sequence Tag ID=0, all in DB_NAME will be retrieved.\n");

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

print_error_page(char *background, char *foreground, char *linkground,
		char *font, char *title)
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

print_main_page(char *background, char *foreground, char *linkground,
		char *font, char *title)
{
	printf("Content-Type: text/html\n\r\n\r");
	printf("<HTML><HEAD><TITLE>%s</TITLE></HEAD>",title);
	printf("<BODY BGCOLOR=%s TEXT=%s LINK=%s VLINK=%s ALINK=%s>",
		background, foreground, linkground,linkground,linkground);
	printf("%s<FONT COLOR=%s>",font,foreground);
	printf("Incorrect Option");
 	printf("</FONT></FONT>");
	printf("</BODY></HTML>");
}

#define MAX_QUERYSIZE	64000

char noabstract=0;
char onlycount=0;

int main(int argc, char **argv)
{
char ignore=0;
long ostid2=0;
long clusterid2=0;
char found5=0;
char keyword[255]={""};
char platestr[255]={""};
char protostr[255]={""};
char subsetname[64]={""};
int curfig=1;
int maxevalue_prot=-1;
int minevalue_prot=-251;
int maxpercentid_prot=100;
int minpercentid_prot=0;
int maxevalue_nuc=-1;
int minevalue_nuc=-251;
int maxpercentid_nuc=100;
int minpercentid_nuc=0;
char pfdesc[255]={""};
char unionstarted=0;
char protalready=0;
char protquery[16000]={""};
char protoquery[255]={""};
char useprotocols=0;
int protomode=0;
long proto_id=0;
char platequery[255]={""};
char useplates=0;
int platemode=0;
long plate_id=0;
long subsetid=0;
long protmask=0;
long nucmask=0;
long excmask=0;
char byname=0;
char foundinc=0;
char foundexc=0;
char *result_p=NULL;
char processed= -1;
DB_TISSUE *ti=NULL;
long plateid=0;
int oo=0;
char kwq[1024]={""};
char haskw=0;
char onlyprotein=0;
int isprotein= -1;
long nbignore=0;
char *strptr=NULL;
char title[255]={""};
int stype=1;
long nbshown=0;
char forcepfam=0;
char hidemasked=0;
short evalmin= -251;
int percmin=0;
int percmax=100;
int per=0;
short evalmax= 0;
int si=0;
long triggerat= -1;
int maxhitpercluster=1;
char onlybesthit=0;
int totalclusters=0;
char querycmd[MAX_QUERYSIZE]={""};
char queryorder[255]={""};
int orderby=0;
int nbsubsets=0;
char blastfile[255]={""};
char *tmpgif=NULL;
char *ocsequence=NULL;
char ostdir[255]={""};
FILE *fd=NULL;
char ocdir[255]={""};
char description[255]={""};
char lastname[64]={""};
DBPROCESS *dbproc=NULL;
DBPROCESS *dbproc2=NULL;
RETCODE rt;
char already=0;
char orderstr[255]={""};
PFAM_HIT *pf=NULL;
int list_by=0;
char giffile[255]={""};
long ostid=0;
long ocid=0;
long oldocid=0;
MULTIFORM *mf=NULL;
char fileloaded=0;
char isdone=0;
long k=0;
char filename[255]={""};
char mimetype[255]={""};
long ii=0;
PAIR *p=NULL;
unsigned char cc=0;
long j=0;
PFAM_INFO *ppf=NULL;
char *c=NULL;
int cl=0;
long i=0;
long n=0;
char buf[1024]={""};
int fdd=0;
int fdi=0;
time_t Sometime=0;
char strtmp2[255]={""};
char strtmp3[255]={""};
int pos=0;
int pos2=0;
char strtmp[32000]={""};
char useit=0;
char stest[255]={""};
long nb_id=0;
char filepath[255]={""};
int iii=0;
long an_id=0;
char *all_line=NULL;
int nb_option=0;
char dbn[64]={""};
char kwquery[255]={""};
char *c2;
int mm=0;
int lane=0;
int well=0;
int count=0;
char ispost=1;
long ticket=0;
register int x,m=0;
char method=0;	/* POST if 1 then GET */
long pubmask=0;
char done=0;
char found=0;
char line[8096]={""};


  chdir("/tmp");
  strcpy(dbn,argv[0]);
  i=0;
  if (strchr(dbn,'_')!=NULL)
  {
    while (argv[0][i]!='_') i++;
    dbn[i]='\0';
  }
  c2=getenv("REQUEST_METHOD");
  chdir("/tmp");
  if(strncmp(c2,"GET",3)==0)
  {
                c = getenv("QUERY_STRING");
                if(c == NULL) {
                  printf("No query information to decode.\n");
                  exit(1);
                }
                for(x=0; c[0]!='\0';x++)
		{
                  m=x;
                  getword(entries[x].val,c,'&');
                  plustospace(entries[x].val);
                  unescape_url(entries[x].val);
                  getword(entries[x].name,entries[x].val,'=');
                  count++;
                }
		ispost=0;
		nb_option=get_pair_option(entries[0].name);
	method=1;
  }
  else
  {
	ispost=1;
	method=0;
    c=(char *)malloc(1024*sizeof(char));
    if ((c=getenv("CONTENT_LENGTH"))==NULL)
    {
       printf("Can't read CONTENT_LENGTH\n");
       exit(0);
    }
    cl=atoi(c);

    all_line=(char *)malloc(cl*sizeof(char)+1);
    strcpy(all_line,"");
    Sometime=time(&Sometime);
    sprintf(strtmp,"%s/answers.%ld",UPLOAD_DIR,Sometime);
    fdd=open(strtmp,O_CREAT|O_WRONLY|O_TRUNC,0666);
    j=0;
    while ((ii=fgetc(stdin))!=EOF)
    {
	cc=(unsigned char)ii;
	all_line[j++]=(char)cc;
	write(fdd,&(cc),1);
    }
    close(fdd);
    all_line[j]='\0';
    mf=(MULTIFORM *)load_multiform(all_line);
#if 0
    printf("Content-Type: text/html\n\r\n\r");
#endif
    if (mf->nbpairs==0)
    {
	free_multiform(mf);
	print_main_page("BLACK","WHITE","#a09090",TIMES_10,"Main Page");
    }
    else
    {
	p=(PAIR *)(mf->pairs[0]);
	nb_option=get_pair_option(p->name);
    }
    p=mf->pairs[0];	/* Get first option */
  }
  chdir("/tmp");
  config_get_lex(dbn); 
  if ((DB_SPECIAL_TOF)&&
     (strcmp(DB_USER,"cperson")==0))
  {
	DB_HAS_PRODUCTION=0;
	DB_SPECIAL_MGRI=1;
  }
  if (strcmp(DB_USER,"board")==0)
		DB_HAS_PRODUCTION=0;
#if 0
  if (ispost)
	printf("Content-Type: text/html\n\r\n\r");
#endif

  if (count==0)
  {
        check_messages();
	printf("Content-Type: text/html\n\r\n\r");
	printf("<HTML><HEAD><TITLE>%s Database & Lexicon Automated Sequence Analysis Package - Copyright(c)1999 Lexicon Genetics Inc.</TITLE>",DB_HTML_NAME);
        printf("</HEAD>");
        printf("<FRAMESET ROWS=\"35,100%%,40\" BORDER=0>\n");
	printf("<FRAME name=ACTIVETOP src=%s?ACTIVETOP=1 noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",DB_NAME);
        printf("<FRAMESET ROWS=\"0,100%%,0\" BORDER=0>\n");
	printf("<FRAME name=LEFTFRAME src=%s/noframe.html noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",LEXDB_HTDOCS_DIR);
	printf("<FRAME name=MAINFRAME src=%s?MAIN_PAGE=1 noresize scrolling=auto marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",DB_NAME);
	printf("<FRAME name=RIGHTFRAME src=%s/noframe.html noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",LEXDB_HTDOCS_DIR);
        printf("</FRAMESET>\n");
	printf("<FRAME name=ACTIVEBOT src=%s?ACTIVEBOT=1 noresize scrolling=no marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>",DB_NAME);
        printf("</FRAMESET>\n");
        printf("</HTML>");
        exit(0);
  }
  switch(nb_option)
  {
    case 0:
	printf("Content-Type: text/html\n\r\n\r");
	printf("<HTML><HEAD><TITLE>Lexicon ASAP! (c)1998,99 Lexicon Genetics Inc.[%s Database]</TITLE><META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\"></HEAD><NOSCRIPT>The ASAP Web Pages uses Javascript functionality. Please enable Javascript in your browser Preferences or contact your system administrator for further help.</P><P ALIGN=JUSTIFY>You will not be able to use all the software features until it is Enabled.</P></NOSCRIPT>",DB_HTML_NAME,LEXDB_HTDOCS_DIR);
	printf("<BODY BGCOLOR=black LINK=white VLINK=white ALINK=red TEXT=white><P><BR><BR><BR><CENTER>%s<FONT COLOR=#209090>Lexicon</FONT> <FONT COLOR=#20a0a0>Automated</FONT> <FONT COLOR=#209090>Sequence</FONT> <FONT COLOR=#209090>Analysis</FONT> <FONT COLOR=#209090>Package</FONT></FONT><CENTER><BR><BR><BR><IMG SRC=%s/gifs/asap/%s ALT='Welcome to the %s Database'>",TIMES_18,LEXDB_HTDOCS_DIR,DB_LOGOGIF_FILE,DB_HTML_NAME);
	printf("</BODY></HTML>");
	exit(0);
	break;
    case 1:
	chdir("/tmp");
	if (method==1)
	{
	    if (sscanf(entries[0].val,"%ld",&ostid)!=1)
		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Sequence Information Query");
#if 0
	    printf("<SCRIPT LANGUAGE=\"JavaScript\">\n\n<!--\nif (top == self) self.location.href = \"%s/noframe.html\";\n// -->\n</SCRIPT>",LEXDB_HTDOCS_DIR);
#endif
	    print_cleartop(1,0,1,"Sequence Information Page","white","black");
	}
	else
	{
	    if (sscanf(p->value,"%ld",&ostid)!=1)
		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Sequence Information Query");
	    print_cleartop(1,0,1,"Sequence Information Page","white","black");
	}
	if (method==0) free_multiform(mf);
	print_sequence_page(ostid);
	break;
    case 2:
    case 13:
    case 14:
	setbuf(stdout,0);
        if (method==1)
	{
	   if (sscanf(entries[0].val,"%ld",&ocid)!=1)
 		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Cluster Information Query");
	}
	else
	{
	   if (sscanf(p->value,"%ld",&ocid)!=1)
 		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Cluster Information Query");
	}
	sprintf(strtmp,"Cluster Information Page for %s%ld",DB_CPREFIX, ocid);
	print_cleartop(1,0,1,strtmp,"white","black");
        if (ocid>0) print_cluster_page(ocid);
        else
		printf("No such Cluster.\n");
	break;
    case 3:
	if (strncpy(kwquery, p->value,254)==NULL)
                print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Keyword Search Query");
 	kwquery[254]='\0';
        if (strlen(kwquery)==0) print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Substance Names Search Query");
        print_cleartop(1,0,1,"Keyword Search Results","white","black");
        printf("Keyword Search Results for [%s]\n",kwquery);
        break;
    case 4:
	if (strncpy(kwquery, p->value,254)==NULL)
                print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Substance Names Search Query");
 	kwquery[254]='\0';
        if (strlen(kwquery)==0) print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Substance Names Search Query");
        print_cleartop(1,0,1,"ASAP Substance Names Search Results","white","black");
        printf("PubMed Substance Names Search Results for [%s]\n",kwquery);
        break;
    case 5:
	sprintf(strtmp,"Lexicon ASAP - %s DB Information",DB_HTML_NAME);
	print_cleartop(1,0,1,strtmp,"white","black");
/*
	  print_dbcontents();*/
	if (DB_HAS_PRODUCTION)
	{
	  print_dbcontents_page();
	}
        else
	{
          dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PROD_SERVER,DB_SYBASE_PROD_NAME);
          dbproc2=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PROD_SERVER,DB_SYBASE_PROD_NAME);
          report_subsets(dbproc,dbproc2);
          report_blastruns(dbproc,dbproc2);
          printf("<BR>&nbsp;");
          /*report_legendball(TIMES_8);*/
          dbexit();
          exit(0);
	}
        break;
    case 6:
	print_cleartop(1,0,1,"ASAP Advanced Searches Page","white","black");
        printf("%s Advanced Searches Page\n",DB_HTML_NAME);
        break;
    case 7:
	sprintf(strtmp,"%s Cluster List Search Control Panel",DB_HTML_NAME);
	print_cleartop3(1,0,1,strtmp,"white","black");
        dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
	print_cluster_list_search(dbproc);
	dbexit();
        break;
    case 8:
	print_cleartop(1,0,1,"ASAP PubMed Substance Names List","white","black");
        printf("%s Related PubMed Substance Names List\n",DB_HTML_NAME);
        break;
    case 9:
	sprintf(strtmp,"LexGenes from the %s Database",DB_HTML_NAME);
	print_cleartop(1,0,1,strtmp,"white","black");
	printf("<CENTER><BR>%s<FONT COLOR=white>%s LexGenes & Tissue Specific Expression Analysis</FONT></FONT><P></CENTER>",TIMES_14,DB_HTML_NAME);
	make_sub_cell("white","white","#209090");
	printf("%s<FONT COLOR=black>",TIMES_10);
	printf("<P ALIGN=JUSTIFY><I>The following options are related to LexGenes annotations and their gene expression-based experiments (i.e. assays). Each assay consists of a selection of available tissues used in a particular tissue panel. Once performed, such experiment will provide expression level information for a LexGene in each of these tissues. The user can provide results only for <U>already defined</U> assays and upload the corresponding expression analysis image from which these results were deduced.</P>",DB_NAME);
	printf("<U>A typical procedure is as follow:</U><BR>");
	printf("<P ALIGN=JUSTIFY><U>1.</U> Create a new lexgene for a cluster by specifying which %s will be the representative of this cluster for expression analysis (i.e. or %s from which primers were designed).</P>",DB_PREFIX, DB_PREFIX);
	printf("<P ALIGN=JUSTIFY><U>2.</U> Define a new tissue panel assay linked to such LexGene. Please enter each \"lane\" corresponding tissue in the proposed form (please specify these tissues from the left to the right columns). You will be able to also specific Control- and Control+ lanes.</P>");
	printf("<P ALIGN=JUSTIFY><U>3.</U> Enter the analysis results of such an <I>Expression Levels Assay</I> by specifying the level of expression for each tissue previously selected. Such result annotations can be any from such possible choices as: <I>Not Assayed</I>, <I>No Signal</I>, <I>Very Weak or Questionable Signal</I>, <I>Signal Definitively Present</I>, <I>Strong Signal</I> or <I>Very Strong Signal</I>.");
	printf("Alternatively, such levels of expression can be specified as a single real number between 0 and 1 (or relative intensity). These expression results needs to be submitted along with a <U>file</U> containing either an image of the expression assay (GIF, TIFF, JPG) or a document including both an image and some additionnal comments or legends (Word Document).</P>");
	printf("</FONT></FONT>");
	close_cell();
	printf("<P>");
	make_sub_cell("white","white","#209090");
	printf("<CENTER>%s<FONT COLOR=black>",TIMES_10);
	printf("To list all defined LexGenes, <A HREF=%s?LIST_LEXGENES=1><FONT COLOR=blue>please follow this link</FONT></A>",DB_NAME);
	printf("</FONT></FONT></CENTER>");
	close_cell();
	printf("<P>");
	make_sub_cell("white","white","#209090");
	printf("<CENTER>%s<FONT COLOR=black>",TIMES_10);
	printf("To create a <I>New LexGene</I>, <A HREF=%s?NEW_LEXGENE=1><FONT COLOR=blue>please follow this link</FONT></A>",DB_NAME);
	printf("</FONT></FONT></CENTER>");
	close_cell();
	printf("<P>");
	make_sub_cell("white","white","#209090");
	printf("<CENTER>%s<FONT COLOR=black>",TIMES_10);
	printf("To define a new <I>Expression Levels Assay</I> for an existing LexGene, <A HREF=%s?NEW_ASSAY=1><FONT COLOR=blue>please follow this link</FONT></A>",DB_NAME);
	printf("</FONT></FONT></CENTER>");
	close_cell();
	printf("<P>");
	make_sub_cell("white","white","#209090");
	printf("<CENTER>%s<FONT COLOR=black>",TIMES_10);
	printf("To specify a expression level results for a performed <I>Expression Levels Assay<I>, <A HREF=%s?NEW_ASSAYRESULTS=1><FONT COLOR=blue>please follow this link</FONT></A>",DB_NAME);
	printf("</FONT></FONT></CENTER>");
	close_cell();
	printf("<P>");
	make_sub_cell("white","white","#209090");
	printf("<CENTER>%s<FONT COLOR=black>",TIMES_10);
	printf("To list LexGenes based on their tissue expression pattern, <A HREF=%s?SEARCH_LEXGENES=1><FONT COLOR=blue>please follow this link</FONT></A>",DB_NAME);
	printf("</FONT></FONT></CENTER>");
	close_cell();
        break;
    case 10:
#if 0
	print_cleartop(1,0,1,"ASAP Online Help","black","white");
        printf("ASAP Online Help\n");
        break;
#endif
	/* TEMPORARY DEBUG BUTTON */
	sprintf(strtmp,"%s Cluster List Search Control Panel",DB_HTML_NAME);
        print_cleartop3(1,0,1,strtmp,"white","white");
        dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
        newprint_cluster_list_search(dbproc);
        dbexit();
        break;
#if 0
    case 11:
        print_cleartop(1,0,1,"ASAP Technical-Support Page","white","black");
        printf("ASAP Technical-Support\n");
        break;
#endif
    case 30:	/* SHOW_GIF_BYSTR */
	sprintf(giffile,"%s/tmp/%s",LEXDB_ROOT_DIR,entries[0].val);
        if ((fdd=open(giffile,O_RDONLY))<0)
        {
                printf("Content-Type: text/html\n\r\n\r");
                printf("Can not display image\n");
                exit(0);
        }
        setbuf(stdout,0);
        printf("Content-Type: Image/gif%c%c",10,10);
        while ((n=read(fdd,buf,1024))>0)
                        write(1,buf,n);
        close(fdd);
        remove(giffile);
        exit(0);
        break;
    case 12:
	if (sscanf(entries[0].val,"%ld",&i)!=1)
	{
		printf("Content-Type: text/html\n\r\n\r");
		printf("This picture can not be displayed out of its frame context<BR>Please push the Back button now.\n");
                exit(0);
	}
	sprintf(giffile,"%s/tmp/%ld.gif",LEXDB_ROOT_DIR,i);
 	if ((fdd=open(giffile,O_RDONLY))<0)
	{
        	printf("Content-Type: text/html\n\r\n\r");
		printf("Can not display image\n");
		exit(0);
	}
	setbuf(stdout,0);
        printf("Content-Type: Image/gif%c%c",10,10);
        while ((n=read(fdd,buf,1024))>0)
                        write(1,buf,n);
        close(fdd);
	remove(giffile);
        exit(0);
	break;
   case 15:
        if ((strncmp(entries[1].name,"PLATE_ID",8)!=0)||
                    (strncmp(entries[2].name,"LANE",4)!=0)||
                    (strncmp(entries[3].name,"WELL",4)!=0))
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
        if ((sscanf(entries[1].val,"%ld",&i)!=1)||
                    (sscanf(entries[2].val,"%d",&lane)!=1)||
                    (sscanf(entries[3].val,"%d",&well)!=1))
		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
        sprintf(strtmp,"%s/databases/%s/ABIs/%s%ld/%.2d-%d", LEXDB_ROOT_DIR,DB_NAME, DB_PLATE_PREFIX,i,lane,well);
#if 0
	printf("Content-Type: text/html\n\r\n\r");
	printf("ABI File=[%s]<BR>\n",strtmp);
	exit(0);
#endif
        if ((fdd=open(strtmp,O_RDONLY))<0)
        {
	  strcat(strtmp,".abi");	/* Sometime the .abi is not there */
	  if ((fdd=open(strtmp,O_RDONLY))<0)
	  {
		sprintf(strtmp2,"Can not find ABI File [%s]\n", strtmp);
		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,strtmp2);
	  }
        }
        setbuf(stdout,0);
        printf("Content-Type: application/x-abi\n\n");
        while ((n=read(fdd,buf,1024))>0)
                        write(1,buf,n);
 	fflush(stdout);
        close(fdd);
        exit(0);
	break;
   case 16:

	sprintf(strtmp,"Lexicon ASAP - %s Available PFAM Hits Searches",DB_HTML_NAME);
	print_cleartop(1,0,1,strtmp,"white","black");
	print_pfamsearch_page();
        break;
	
   case 17:
		if (strlen(LEXDB_SYBASE_PFAMDB_NAME)<1)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"No PFAM Database Available");
		sprintf(strtmp,"%s Hit Protein Families in PFAM",DB_HTML_NAME);
        	print_cleartop(1,0,1,strtmp,"white","black");
/*
		if (sscanf(entries[0].val,"%d",list_by)!=1)
			list_by=0;
        	else	list_by=1;
*/
		list_by=1;
  		make_sub_cell("black","black","#209090");
  		printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
  		printf("<TH BGCOLOR=black><IMG SRC=%s/gifs/asap/%s WIDTH=%ld HEIGHT=%ld ALT='You are accessing the %s Database'></TH>",
        	        LEXDB_HTDOCS_DIR,DB_ICONGIF_FILE,
        	        DB_ICONGIF_WIDTH, DB_ICONGIF_HEIGHT,
        	        DB_HTML_NAME);
  		printf("</TH><TH BGCOLOR=black>");

  		switch(list_by)
  		{
		   case 0:
			printf("%s<FONT COLOR=white><B>List of Hit Protein Families in PFAM DB<BR>(ordered by description)</B></FONT></FONT>\n",TIMES_14);
        	        strcpy(orderstr,"description");
			break;
		   case 1:
			printf("%s<FONT COLOR=white><B>List of Hit Protein Families in PFAM DB<BR>(ordered by name)</B></FONT></FONT>\n",TIMES_14);
        	        strcpy(orderstr,"name");
			break;
  		}
  		printf("</TH>");
  		printf("<TH BGCOLOR=black><IMG SRC=%s/gifs/asap/%s WIDTH=%ld HEIGHT=%ld ALT='You are accessing the %s Database'></TH>",
        	        LEXDB_HTDOCS_DIR,DB_ICONGIF_FILE,
        	        DB_ICONGIF_WIDTH, DB_ICONGIF_HEIGHT,
        	        DB_HTML_NAME);
  		printf("</TR></TABLE>");
  		close_cell();
		printf("<BR>");
        	already=0;
		dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PFAMDB_SERVER,LEXDB_SYBASE_PFAMDB_NAME);
		if (strlen(entries[0].val)>0)
		{
			sprintf(strtmp,"[%c%c]%%",
				toupper(entries[0].val[0]),
				tolower(entries[0].val[0]));
		}
		else
			strcpy(strtmp,"");
        	print_pfamilies_table(dbproc,strtmp);
		dbexit();
		exit(0);
        	break;
	case 18:
		if (ispost)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		sprintf(strtmp,"%s Hits in the \"%s\" Protein Family",
			DB_HTML_NAME,entries[0].val);
                print_cleartop(1,0,1,strtmp,"white","black");
        	printf("<CENTER>");
        	make_sub_cell("black","black","#209090");
        	printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
        	printf("<TH BGCOLOR=black><IMG SRC=%s/gifs/asap/%s WIDTH=%ld HEIGHT=%ld ALT='You are accessing the %s Database'></TH>",
                LEXDB_HTDOCS_DIR,DB_ICONGIF_FILE,
                DB_ICONGIF_WIDTH, DB_ICONGIF_HEIGHT,
                DB_HTML_NAME);
        	printf("</TH><TH BGCOLOR=black>");
		printf("%s<FONT COLOR=white><B>%s</B></FONT></FONT>\n",TIMES_18,strtmp);

        	printf("</TH>");
        	printf("<TH BGCOLOR=black><IMG SRC=%s/gifs/asap/%s WIDTH=%ld HEIGHT=%ld ALT='You are accessing the %s Database'></TH>",
                LEXDB_HTDOCS_DIR,DB_ICONGIF_FILE,
                DB_ICONGIF_WIDTH, DB_ICONGIF_HEIGHT,
                DB_HTML_NAME);
        	printf("</TR></TABLE>");
        	close_cell();
        	printf("<BR>");

		dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PFAMDB_SERVER,LEXDB_SYBASE_PFAMDB_NAME);
        	print_pfamilies_table(dbproc,entries[0].val);
		print_pfam_hits(entries[0].val, 0);
		exit(0);

                dbsqlexec(dbproc);
		already=0;
		oldocid=0;
                while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS)
                {
                    if (rt==SUCCEED)
                    {
                        dbbind(dbproc,1,STRINGBIND,(DBINT)0,(BYTE DBFAR *)(pf->description));
                        dbbind(dbproc,2,SMALLBIND,(DBINT)0,(BYTE *)&(pf->pvalue_e));
                        dbbind(dbproc,3,REALBIND,(DBINT)0,(BYTE *)&(pf->pvalue_v));
                        dbbind(dbproc,4,INTBIND,(DBINT)0,(BYTE *)&(pf->ostid));
                        dbbind(dbproc,5,INTBIND,(DBINT)0,(BYTE *)&ocid);
                        dbbind(dbproc,6,INTBIND,(DBINT)0,(BYTE *)&pubmask);
                        while (dbnextrow(dbproc)!=NO_MORE_ROWS)
                        {
			   if (!already)
			   {
				make_sub_cell("black","black","#209090");
				printf("<TABLE BORDER=0 WIDTH=100%%><TR>\n");
				printf("<TH BGCOLOR=#209090 WIDTH=20%%>Sequence Tag ID</TH>");
				printf("<TH BGCOLOR=#209090 WIDTH=20%%>Cluster Tag ID</TH>");
				printf("<TH BGCOLOR=#209090 WIDTH=20%%>P-Value</TH>");
				printf("<TH BGCOLOR=#209090 WIDTH=40%%>Recompute (2 possible output formats)</TH></TR>");
				already=1;
			   }
			   printf("<TR><TH BGCOLOR=white WIDTH=20%%><A HREF=%s?SHOW_SEQUENCE=%ld TARGET=MAINFRAME><FONT COLOR=blue>%s%ld</FONT></A></TH>",
					DB_NAME, pf->ostid,
					DB_PREFIX,pf->ostid);
			   printf("<TH BGCOLOR=white WIDTH=20%%><A HREF=%s?SHOW_CLUSTER=%ld TARGET=MAINFRAME><FONT COLOR=blue>%s%ld</FONT></A>",
					DB_NAME, ocid,
					DB_CPREFIX,ocid);
			   if (pubmask==0)
				printf(" <BLINK><FONT COLOR=red>$$</FONT></BLINK>");
			   printf("</TH>");
			   if (oldocid==ocid)
                                 printf("<TH BGCOLOR=white WIDTH=20%%><FONT COLOR=black>%.1fe%d</FONT>", pf->pvalue_v, pf->pvalue_e);
			   else
			   {
				oldocid=ocid;
                                printf("<TH BGCOLOR=white WIDTH=20%%><FONT COLOR=#209090><BLINK>%.1fe%d</BLINK></FONT>", pf->pvalue_v, pf->pvalue_e);
			   }
		           printf("</TH>");
                           printf("<TH BGCOLOR=#209090 WIDTH=40%%><A HREF=pfam.cgi?dbn=%s&tstid=%ld&family=%s&format=blast TARGET=MAINFRAME>BLAST</A>/<A HREF=pfam.cgi?dbn=%s&tstid=%ld&family=%s&format=paracel TARGET=MAINFRAME>Paracell</A></TH></TR>", DB_NAME, pf->ostid, entries[0].val,DB_NAME, pf->ostid, entries[0].val);

#if 0
				printf("%s%ld [%s%ld] (%.1fe%d)<BR>",
					DB_PREFIX,pf->ostid,
					DB_CPREFIX, ocid,
					pf->pvalue_v, pf->pvalue_e);
#endif
			}
		    }
		}
		if (already)
		{
			printf("</TABLE>");
			close_cell();
		}
		dbexit();
		break;
    case 19:
		if (sscanf(entries[0].val,"%ld",&ocid)!=1)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		sprintf(strtmp,"Multiple Alignment [ALIGNM] for 5 longuest %ss in %s%ld", DB_PREFIX, DB_CPREFIX,ocid);
        	setbuf(stdout,0);
                print_cleartop(1,0,1,strtmp,"white","black");
                printf("<CENTER>");
		get_oc_dir(ocdir,ocid);
		sprintf(strtmp,"%s/%s%ld.alignm",ocdir,DB_CPREFIX,ocid);
        	if ((fdd=open(strtmp,O_RDONLY))<0)
        	{
                	printf("No ALIGNM file for %s%ld\n",DB_CPREFIX,ocid);
                	exit(0);
        	}
                make_sub_cell("black","black","#209090");

                printf("<TABLE BORDER=0 WIDTH=100%%><TR><TD BGCOLOR=white WIDTH=100%%>");
		printf("<PRE>%s<FONT COLOR=black>",COURIER_10);
        	while ((n=read(fdd,buf,1024))>0) write(1,buf,n);
		printf("</FONT></FONT></PRE>");
		printf("</TD></TR></TABLE>");
		close_cell();
        	close(fdd);
		break;
    case 20:
                printf("Content-Type: text/html\n\r\n\r");
		printf("UNDER DEVELOPMENT<P>");
		exit(0);
		break;
    case 21:
		if (sscanf(entries[0].val,"%ld",&ostid)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		if (ispost)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
                sprintf(strtmp,"Confirmation Page for %s%ld Invalidation<P>",
                        DB_HTML_NAME,entries[0].val);
                print_cleartop(1,0,0,strtmp,"white","black");
		printf("<CENTER>");
		printf("%s<FONT COLOR=white><BR><B>Please confirm that %s%ld should be invalid or go back to the previous page...</B></FONT></FONT>",TIMES_14,DB_PREFIX,ostid);
		
 		printf("<FORM METHOD=GET ACTION=%s TARGET=MAINFRAME>%s<INPUT TYPE=HIDDEN NAME=CONFIRM_INVALID_SEQUENCE VALUE=%ld><INPUT TYPE=IMAGE SRC=%s/gifs/asap/invalidateseq.gif NAME=\"Click To Confirm This Sequence Tag Invalidation\" BORDER=0></FONT></FORM>",DB_NAME,TIMES_8,ostid,LEXDB_HTDOCS_DIR);
                exit(0);
		break;
    case 22:
		if (sscanf(entries[0].val,"%ld",&ostid)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
                if (ispost)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
                sprintf(strtmp,"Invalidation Results Page for %s%ld<P>",
                        DB_HTML_NAME,entries[0].val);
                print_cleartop(1,0,0,strtmp,"white","black");
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		dbfcmd(dbproc,"update OST_Table set valid=0 where valid=1 and cluster_id<1 and id=%ld", ostid);
		dbsqlexec(dbproc);
		while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS) continue;
                printf("<CENTER>");
                printf("%s<FONT COLOR=white><BR><B>%s%ld has been invalidated. Please push the Back button twice or <A HREF=javascript:history.go(-2)><FONT COLOR=white>Follow This Link</FONT></A><P>",TIMES_12,DB_PREFIX,ostid);
                printf("%s<FONT COLOR=white><BR><B>You will also need to push the RELOAD Button to update the Sequence Tag page !!<P>",TIMES_12,DB_PREFIX,ostid);
		get_ost_dir(ostdir,ostid);
		sprintf(strtmp,"%s/%s%ld.invalidby",ostdir,DB_PREFIX,ostid);
		if ((fd=fopen(strtmp,"w"))!=NULL)
		{
			Sometime=time(&Sometime);
			strcpy(strtmp,ctime(&Sometime));
			strtmp[strlen(strtmp)-1]='\0';
			fprintf(fd,"%s %s\n", DB_USER,strtmp);
			fclose(fd);
		}
                exit(0);
		break;
    case 23:	/* Prints out a Plate Diagram */
    case 45:
		plateid=0;
		print_cleartop(1,0,0,strtmp,"white","black");
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		printf("<P><BR>");
		processed=-1;
		if (strstr(entries[0].name,"_BY_NAME")==NULL)
		{
		  byname=0;
		  if (sscanf(entries[0].val,"%ld",&plateid)!=1)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Plate Number");
		  dbfcmd(dbproc,"select processed from Plate_Table where id=%ld", plateid);
		}
		else
		{
		  byname=1;
		  dbfcmd(dbproc,"select processed,id from Plate_Table where name='%s'", entries[0].val);
		}
		dbsqlexec(dbproc);
		while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS)
		{
		    if (rt==SUCCEED)
		    {
			dbbind(dbproc,1,BITBIND,(DBINT)0,(BYTE *)&processed);
			if (byname)
			  dbbind(dbproc,2,INTBIND,(DBINT)0,(BYTE *)&plateid);
			while (dbnextrow(dbproc)!=NO_MORE_ROWS)
				dbcanquery(dbproc);
	 	    }
		}
		switch (processed)
		{
		    case -1:
			make_sub_cell("white","white","#209090");
			if (byname)
				printf("<CENTER>%s<FONT COLOR=black>No Plate with Name [%s] exist in the %s Database</FONT></FONT></CENTER>",TIMES_12,entries[0].val,DB_HTML_NAME);
			else
				printf("<CENTER>%s<FONT COLOR=black>Plate #%ld does not exist in the %s Database</FONT></FONT></CENTER>",TIMES_12,plateid,DB_HTML_NAME);
			close_cell();
			exit(0);
			break;
		    case 0:
			print_plate_files(dbproc,plateid);
			break;
		    case 1:
			print_plate_diagram(dbproc, plateid);
			printf("<P>");
			print_plate_analysis_diagram(dbproc, plateid);
			break;
		}
		dbexit();
		exit(0);
		break;
    case 24:	/* Prints out an alignment portion from a CLUSTER BLAST file */
		/* VIEW BLAST ALIGN */
		if (ispost)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query Method");
		if (strncmp(entries[0].val,DB_CPREFIX,strlen(DB_CPREFIX))!=0)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		pos=strlen(DB_CPREFIX);
                while ((entries[0].val[pos]!='.')&&(pos<strlen(entries[0].val)))
                        pos++;
                if (pos>=strlen(entries[0].val))
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
                strcpy(strtmp,entries[0].val+strlen(DB_CPREFIX));
                strtmp[pos-strlen(DB_CPREFIX)]='\0';
                if (sscanf(strtmp,"%ld",&ocid)!=1)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		get_oc_dir(ocdir,ocid);
		sprintf(filepath,"%s/%s",ocdir,entries[0].val);
		for (i=0; i<strlen(filepath); i++)
		{
			if (entries[0].val[i]==' ') entries[0].val[i]='+';
			if (filepath[i]==' ') filepath[i]='+';
		}
		if ((fd=fopen(filepath,"r"))==NULL)
		{
			sprintf(strtmp,"Can not open BLAST results file");
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,strtmp);
		}
		sprintf(strtmp,"BLAST Alignment of Query=%s%ld vs. Sbjct=%s",
			DB_CPREFIX, ocid, entries[1].val);
                print_cleartop(1,0,0,strtmp,"white","black");
#if 0
		make_sub_cell("#209090","black","#209090");
		printf("<CENTER>%s<FONT COLOR=white ALIGN=TOP><B>%s</B></FONT></FONT> ",TIMES_12,strtmp);
		printf(" <A HREF=\"#\" onClick=\"window.open('%s?SHOW_BLAST_MULTIALIGN=%s','BLAST_MULTIALIGN','scrollbars=yes,resizable=yes,width=700,height=350,top=20,left=20');\"><IMG SRC=\"/lexdb2/gifs/asap/alignment.gif\" ALT=\"Click To View the Multiple Sequence Alignment for %s%ld\" BORDER=0></FONT></A> ", DB_NAME,  entries[0].val,DB_CPREFIX, ocid);
		printf(" <A HREF=\"javascript:window.close()\"><IMG SRC=%s/gifs/asap/closewindow.gif ALT=\"Close This Window\" BORDER=0></A></TD>\n",LEXDB_HTDOCS_DIR);
		close_cell();
#endif
		done=0; found=0;

		isprotein= -1;
		if (strncmp(entries[2].name,"IS_PROTEIN",10)==0)
		{
		   if (sscanf(entries[2].val,"%d",&isprotein)!=1)
			isprotein= -1;
		}
		if (strncmp(entries[3].name,"SUBSETNAME",10)==0)
		{
		   if (strcpy(subsetname,entries[3].val)==NULL)
			strcpy(subsetname,"");
		}
		sprintf(stest,"ACCESSION:%s ",entries[1].val);
  		make_sub_cell("white","white","#209090");
		printf("%s<FONT COLOR=black><PRE>",COURIER_10);
		display_align(filepath, entries[1].val, (char)isprotein, subsetname);
		printf("</PRE></FONT></FONT>");
		close_cell();
		printf("</BODY></HTML>");
		exit(0);
		break;

    case 25:	/* Prints out a BLAST file in FRAMES */
		if (strcpy(strtmp,entries[0].val)==NULL)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		isprotein= -1;
                if (strncmp(entries[2].name,"IS_PROTEIN",10)==0)
                {
                   if (sscanf(entries[2].val,"%d",&isprotein)!=1)
                        isprotein= -1;
                }
		pos=0; while ((strtmp[pos]!='.')&&(pos<strlen(strtmp))) pos++;
		if (pos==strlen(strtmp))
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		strcpy(strtmp2,strtmp);
		strtmp2[pos]='\0';
		strcpy(strtmp2,strtmp2+strlen(DB_CPREFIX));
		if (sscanf(strtmp2,"%ld",&ocid)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Can't load cluster number");
		get_oc_dir(ocdir,ocid);
		sprintf(strtmp2,"%s/%s",ocdir,strtmp);
		if ((fd=fopen(strtmp2,"r"))==NULL)
		{
			sprintf(title,"Can't load %s",strtmp2);

                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,title);
		}
		fclose(fd);
		printf("Content-Type: text/html\n\r\n\r");
		printf("<HTML><HEAD><TITLE>Lexicon HumanTST Database & Automated Sequence Analysis Package - Copyright(c)1999 Lexicon Genetics Inc.</TITLE>\n");
		printf("</HEAD>");
  		printf("<FRAMESET ROWS=\"200,*\" BORDER=0>\n");
       		printf("<FRAME name=HITSFRAME src=%s?SHOW_BLAST_HITS=%s noresize scrolling=auto marginwidth=0 marginheight=0 frameborder=no borderheight=0 borderwidth=0>\n",DB_NAME,strtmp);
       		printf("<FRAME name=ALIGNFRAME src=%s?SHOW_BLAST_ALIGN=%s&BLAST_HIT=TOP&IS_PROTEIN=%d# scrolling=auto marginwidth=0 marginheight=0 frameborder=0 borderheight=0 borderwidth=0>\n",DB_NAME,strtmp,isprotein);
    		printf("</FRAMESET>\n");
		printf("</HTML>");
                exit(0);
		break;
    case 26:	/* Prints out the BLAST Hits topframe */ /* SHOW_BLAST */
		print_cleartop(1,0,0,strtmp,"white","black");
		if (strcpy(strtmp,entries[0].val)==NULL)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		isprotein= -1;
                if (strncmp(entries[2].name,"IS_PROTEIN",10)==0)
                {
                   if (sscanf(entries[2].val,"%d",&isprotein)!=1)
                        isprotein= -1;
                }

		for (i=0; i<strlen(strtmp); i++)
			if (strtmp[i]==' ') strtmp[i]='+';
                pos=0; while ((strtmp[pos]!='.')&&(pos<strlen(strtmp))) pos++;
                if (pos==strlen(strtmp))
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
                strcpy(strtmp2,strtmp);
                strtmp2[pos]='\0';
                strcpy(strtmp2,strtmp2+strlen(DB_CPREFIX));
                if (sscanf(strtmp2,"%ld",&ocid)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Can't load cluster number");
                get_oc_dir(ocdir,ocid);
                sprintf(strtmp2,"%s/%s",ocdir,strtmp);
                if ((fd=fopen(strtmp2,"r"))==NULL)
                {
                        sprintf(title,"Can't load %s",strtmp2);

                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,title);
                }
                if ((fd=fopen(strtmp2,"r"))==NULL)
		done=0; already=0;
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		make_sub_cell("white","white","#209090");
		printf("<PRE>%s<FONT COLOR=black>",COURIER_10);
		while ((!done) && (!feof(fd)))
		{
		   if (fgets(line,1024,fd)!=NULL)
		   {
		     if ((!already)&&(strncmp(line,"Query=",6)==0))
			already=1;
		     if (already)
		     {
			if (line[0]=='>')
				done=1;
			else
			{
		           if (strncmp(line,"Sequences producing",19)==0)
			   	found=1;
			   else
			   if (found)
			   {
			     pos=0;
		             while ((line[pos]!=' ')&&(pos<strlen(line)))
					 pos++;
			     if (pos!=strlen(line))
			     {
				found5=0;
				ignore=0;
			        strcpy(strtmp2,line);
			        strtmp2[pos]='\0';
				/* Only for clustering */
			        if
				(strncmp(DB_PREFIX,strtmp2,strlen(DB_PREFIX))==0)
				{
				  if (sscanf(strtmp2+strlen(DB_PREFIX),"%ld",&ostid2)==1)
				  {
					found5=1;
					dbfcmd(dbproc,"select cluster_id from OST_Table where id=%ld",ostid2);
					dbsqlexec(dbproc);
					while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS)
					{
					   if (rt==SUCCEED)
					   {
					     dbbind(dbproc,1,INTBIND,(DBINT)0,(BYTE *)&clusterid2);
					     while (dbnextrow(dbproc)!=NO_MORE_ROWS) dbcanquery(dbproc);
					   }
					}
					Sometime=time(&Sometime);
					if (clusterid2>0)
					{
					    if (clusterid2==ocid)
						printf("<A HREF=%s?SHOW_BLAST_ALIGN=%s&BLAST_HIT=%s&IS_PROTEIN=%d TARGET=ALIGNFRAME><FONT COLOR=blue>%s%ld</FONT></A> - ClusterID:<A HREF=%s?SHOW_CLUSTER=%ld TARGET=MAINFRAME><FONT COLOR=blue>%s%ld</FONT></A> ",DB_NAME,strtmp,strtmp2,isprotein,DB_PREFIX,ostid2,DB_NAME,clusterid2,DB_CPREFIX, clusterid2);
					    else
						printf("<A HREF=%s?SHOW_BLAST_ALIGN=%s&BLAST_HIT=%s&IS_PROTEIN=%d TARGET=ALIGNFRAME><FONT COLOR=red>%s%ld</FONT></A> - ClusterID:<A HREF=%s?SHOW_CLUSTER=%ld TARGET=MAINFRAME><FONT COLOR=red>%s%ld</FONT></A> ",DB_NAME,strtmp,strtmp2,isprotein,DB_PREFIX,ostid2,DB_NAME,clusterid2,DB_CPREFIX, clusterid2);
					}
					else
					{
#if 0
						ignore=1;
#else
						printf("<A HREF=%s?SHOW_BLAST_ALIGN=%s&BLAST_HIT=%s&IS_PROTEIN=%d TARGET=ALIGNFRAME><FONT COLOR=blue>%s%ld</FONT></A> - (unclustered) ",DB_NAME,strtmp,strtmp2,isprotein,DB_PREFIX,ostid2);
#endif
					}
					
				  }
				}
				if (!ignore)
				{
			            if (!found5)
			            {
					printf("<A HREF=%s?SHOW_BLAST_ALIGN=%s&BLAST_HIT=%s&IS_PROTEIN=%d TARGET=ALIGNFRAME><FONT COLOR=blue>%s</FONT></A> ",DB_NAME,strtmp,strtmp2,isprotein,strtmp2);
				       strcpy(line,line+strlen(strtmp2)+1);
				         printf("%s", line);
			            }
				    else
				       printf("%s", line+strlen(strtmp2)+1);
				}
			      }
			      else
				printf("%s",line);
			   }
			}
		     }
		   }
		}
		printf("</FONT></FONT></PRE>");
		close_cell();
                fclose(fd);
 		dbexit();
                exit(0);
		break;
    case 27:
		print_cleartop(1,0,0,strtmp,"white","black");
		print_dbcontents();
		exit(0);
		break;
    case 28:
		print_cleartop(1,0,0,strtmp,"white","black");
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PROD_SERVER,DB_SYBASE_PROD_NAME);
        	dbproc2=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PROD_SERVER,DB_SYBASE_PROD_NAME);
		report_subsets(dbproc,dbproc2);
		report_blastruns(dbproc,dbproc2);
		printf("<BR>&nbsp;");
		/*report_legendball(TIMES_8);*/
		dbexit();
		exit(0);
		break;
    case 29:  /* SHOW_BLAST_MULTIALIGN_GIF : call to get_alignment_blastfile */
    case 33:
		if (strncmp(entries[0].val, DB_CPREFIX, strlen(DB_CPREFIX))!=0)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		strcpy(blastfile,entries[0].val);
		for (i=0; i<strlen(blastfile); i++)
			if (blastfile[i]==' ') blastfile[i]='+';
		strcpy(strtmp,blastfile+strlen(DB_CPREFIX));
		pos=0; while ((pos<strlen(strtmp))&&
			      (strtmp[pos]!='.')) pos++;
		strtmp[pos]='\0';
		
		if (sscanf(strtmp,"%ld",&ocid)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		get_oc_dir(ocdir, ocid);
		sprintf(strtmp,"%s/%s",ocdir,blastfile);
		sprintf(blastfile,"%s/%s%ld", ocdir, DB_CPREFIX, ocid);
		if (strcmp(entries[0].name,"SHOW_BLAST_MULTIALIGN_GIF")==0)
		{
		  sprintf(title,"Alignment from %s%ld", DB_CPREFIX,ocid);
		  if ((tmpgif=(char *)get_alig_blast(strtmp, blastfile, 250,"/tmp", title))==NULL)
#if 0
		  if ((tmpgif=(char *)get_aligment_blastfile(strtmp, blastfile, 250))==NULL)
#endif
		  {
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Can't open graphic alignment picture");
		  }
		  output_gif_file(tmpgif);	/* Send the gif over */
        	  remove(giffile);
		  remove(tmpgif);
		  free(tmpgif);
		}
		else
		{
		  if ((strptr=(char *)get_Multi_Fasta(strtmp,blastfile,1000,"/tmp"))!=NULL)
		  {
		    printf("Content-Type: text/html\n\r\n\r");
		    printf("<HTML><HEAD>\n");
		    printf("<!--LEXICON ASAP (c)1999 COPYRIGHT LEXICON GENETICS INC - ALL RIGHTS RESERVED -->\n");
		    printf("</HEAD>");
		    printf("<BODY BGCOLOR=WHITE TEXT=BLACK>");
		    printf("%s",COURIER_10);
		    printf("<PRE>%s\n</PRE>", strptr);
		    free(strptr); strptr=NULL;
		    printf("</PRE></FONT>");
		    printf("</BODY></HTML>");
		  }
		  else
			printf("Text Alignment can not be displayed at this time.");
		}
        	exit(0);
		break;
    case 31:	/* SHOW_BLAST_MULTIALIGN */
		 if (strncmp(entries[0].val, DB_CPREFIX, strlen(DB_CPREFIX))!=0)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
                strcpy(blastfile,entries[0].val);
                for (i=0; i<strlen(blastfile); i++)
                        if (blastfile[i]==' ') blastfile[i]='+';
                strcpy(strtmp,blastfile+strlen(DB_CPREFIX));
                pos=0; while ((pos<strlen(strtmp))&&(strtmp[pos]!='.')) pos++;
                strtmp[pos]='\0';
                if (sscanf(strtmp,"%ld",&ocid)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		sprintf(strtmp,"FastAlign Multiple Alignment for %s", blastfile);
		print_cleartop(1,0,0,strtmp,"white","black");
		printf("<TABLE BORDER=0><TR>");
		printf("<TD BGCOLOR=black><A HREF=\"javascript:window.close()\"><IMG SRC=%s/gifs/asap/closewindow.gif ALT=\"Close This Window\" BORDER=0></A></TD>\n",LEXDB_HTDOCS_DIR);
		printf("<TD BGCOLOR=black><FONT COLOR=white>%s<B>FastAlign Multiple Alignment for <U>%s</U> BLAST Results File</FONT></FONT></TD>\n",TIMES_12,  blastfile);
		printf("</TR></TABLE>\n");
  		make_sub_cell("white","white","#209090");
		printf("<IMG SRC=%s?SHOW_BLAST_MULTIALIGN_GIF=%s BORDER=0>\n",DB_NAME,blastfile);
		close_cell();

                exit(0);
                break;

    case 32:
		Sometime=time(&Sometime);
		onlyprotein=0;
		sprintf(strtmp,"Cluster List Searches Results %s", ctime(&Sometime));
		strtmp[strlen(strtmp)-1]='\0';	/* Remove the \n from ctime */
		if (sscanf(entries[0].val, "%d", &i)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		print_cleartop2(1,0,0,strtmp,"white","black");
		printf("\n\n<FORM METHOD=GET ACTION=%s>\n",DB_NAME);
		printf("<INPUT TYPE=HIDDEN NAME=CLUSTER_LIST VALUE=%d>\n",i);
		printf("<BR><CENTER>%s<FONT COLOR=white><B>%s</B> ", TIMES_10,strtmp);
		strcpy(querycmd,"");
		strcpy(queryorder," order by OC_Table.id asc");
		triggerat= -1;
		for (i=0; i<count; i++)
		{	 /* Make a pass to know what we order by */
		  if (strcmp(entries[i].name,"TRIGGER")==0)
		  {
			if (sscanf(entries[i].val,"%ld", &triggerat)!=1)
				triggerat= -1;
			if (triggerat<1) triggerat= -1;
#if 0
			else	
			    printf("<INPUT TYPE=HIDDEN NAME=TRIGGER VALUE=%d>\n",triggerat);
#endif
		  }
		  if (strcmp(entries[i].name,"SEQTYPE")==0)
                  {
                        if (sscanf(entries[i].val,"%d", &stype)==1)
                        {
				printf("<INPUT TYPE=HIDDEN NAME=SEQTYPE VALUE=%d>\n",stype);
                        }
			if (stype>1) stype=1;
			if (stype<0) stype=0;
                  }
		  if (strcmp(entries[i].name,"MIN_PERCENT")==0)
                  {
                        if (sscanf(entries[i].val,"%d", &per)==1)
                        {
                                percmin=(short)per;
				printf("<INPUT TYPE=HIDDEN NAME=MIN_PERCENT VALUE=%d>\n",percmin);
                        }
			if (percmin<0) percmin=0;
			if (percmin>=100) percmin=99;
                  }
		  if (strcmp(entries[i].name,"MAX_PERCENT")==0)
                  {
                        if (sscanf(entries[i].val,"%d", &per)==1)
                        {
                                percmax=(short)per;
				printf("<INPUT TYPE=HIDDEN NAME=MAX_PERCENT VALUE=%d>\n",percmax);
                        }
                        if (percmax>100) percmax=100;
                        if (percmax<=0) percmax=1;
                  }
		  if (strcmp(entries[i].name,"ONLYPROTEIN")==0)
		  {
			onlyprotein=1;
			printf("<INPUT TYPE=HIDDEN NAME=ONLYPROTEIN VALUE=1>\n");
		  }
		  if (strcmp(entries[i].name,"NBIGNORE")==0)
		  {
			if (sscanf(entries[i].val,"%ld",&nbignore)!=1)
				nbignore=0;
			else
			{
				if (nbignore<0) nbignore=0;
			}
		  }
		  if (strcmp(entries[i].name,"MIN_EVALUE")==0)
                  {
                        if (sscanf(entries[i].val,"%d", &si)==1)
			{
				evalmin=(short)si;
				printf("<INPUT TYPE=HIDDEN NAME=MIN_EVALUE VALUE=%d>\n",evalmin);
			}
                  }
		  if (strcmp(entries[i].name,"FORCE_PFAM")==0)
			forcepfam=1;
		  if (strcmp(entries[i].name,"MAX_EVALUE")==0)
                  {
                        if (sscanf(entries[i].val,"%d", &si)==1)
                        {
                                evalmax=(short)si;
				printf("<INPUT TYPE=HIDDEN NAME=MAX_EVALUE VALUE=%d>\n",evalmax);
                        }
			if (evalmax>=0) evalmax=0;
			if (evalmax<-250) evalmax=-249;
                  }
		  if (strcmp(entries[i].name,"MAXCLUSTERS")==0)
		  {
			if (sscanf(entries[i].val,"%d",&totalclusters)!=1)
				totalclusters=25;
			else
			{
			   if (totalclusters<1)
				totalclusters=10;
			   if (totalclusters>100)
				totalclusters=100;
			}
			printf("<INPUT TYPE=HIDDEN NAME=MAXCLUSTERS VALUE=%d>\n",totalclusters);
		  }
 		  if (strcmp(entries[i].name,"HIDE_MASKED")==0)
		  {
			printf("<INPUT TYPE=HIDDEN NAME=HIDE_MASKED VALUE=1>\n");
			hidemasked=1;
		  }
		  if (strcmp(entries[i].name,"MAXHITPERCLUSTER")==0)
		  {
			if (sscanf(entries[i].name,"%d",&maxhitpercluster)!=1)
				maxhitpercluster=1;
			printf("<INPUT TYPE=HIDDEN NAME=MAXHITPERCLUSTER VALUE=1>\n");
		  }
		  if (strcmp(entries[i].name,"ONLYBESTHIT")==0)
                  {
			onlybesthit=1;
			printf("<INPUT TYPE=HIDDEN NAME=ONLYBESTHIT VALUE=1>\n");
                  }
		  if (strcmp(entries[i].name,"KEYWORD")==0)
		  {
			if (strcpy(kwq,entries[i].val)!=NULL)
			{
			   haskw=1;
			}
			else 
			   haskw=0;
		  }
		  if (strcmp(entries[i].name,"NOABSTRACT")==0)
		  {
			noabstract=1;
		  }
		  if (strcmp(entries[i].name,"SUBSET")==0)
		  {
			printf("<INPUT TYPE=HIDDEN NAME=SUBSET VALUE=%s>\n",
				entries[i].val);
		  }
		  if (strcmp(entries[i].name,"ORDERBY")==0)
                  {
			if (sscanf(entries[i].val,"%d",&orderby)==1)
			{
			  printf("<INPUT TYPE=HIDDEN NAME=ORDERBY VALUE=%d>\n",orderby);
			  switch (orderby)
			  {
				case 1:	/* By Cluster ID desc. */
				   strcpy(queryorder," order by OC_Table.id desc");
				   break;
				case 2:	/* By EValue, Cluster ID asc */
				   strcpy(queryorder," order by OC_Table.pvalue_e asc, pvalue_v asc, percentid desc, OC_Table.id asc");
				   break;
				case 3: /* By EValue, Cluster ID desc */
				   strcpy(queryorder," order by OC_Table.pvalue_e asc, pvalue_v asc, percentid desc, OC_Table.id desc");
				   break;
				case 4:
				   strcpy(queryorder," order by OC_Table.length desc, OC_Table.id asc");
				   break;
			  }
			}
		  }
		}
		printf("<INPUT BORDER=0 TYPE=IMAGE NAME=\"Click here to view the next %d Clusters matching your search\" SRC=%s/gifs/asap/next.gif></INPUT>",totalclusters,LEXDB_HTDOCS_DIR); printf("</FONT></FONT></CENTER>", TIMES_10,strtmp);
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PROD_SERVER,DB_SYBASE_PROD_NAME);
		if (onlybesthit)
		{
			nbsubsets=0;
			sprintf(querycmd,"select OC_Table.id,OC_Table.seedid,OC_Table.length,OC_Table.%csubset,OC_Table.%cacc,OC_Table.%cpvalue_v, OC_Table.%cpvalue_e, OC_Table.%cpercentid, Subset_Table.seqtype, BLAST_Run_Table.blast_extension,OC_Table.nsubset,OC_Table.psubset,OC_Table.pubmask ", seqstr[stype],seqstr[stype],seqstr[stype],seqstr[stype],seqstr[stype]);
			sprintf(querycmd,"%s from OC_Table, Subset_Table, BLAST_Run_Table where OC_Table.valid>0 and OC_Table.id>0 and OC_Table.npvalue_e<%d and OC_Table.npvalue_e>=%d and Subset_Table.name=OC_Table.nsubset and BLAST_Run_Table.id=Subset_Table.blastrun_id and OC_Table.npercentid>%d and OC_Table.npercentid<%d ",querycmd,evalmax, evalmin, percmin,percmax);
			dbfcmd(dbproc,"select OC_Table.id,OC_Table.seedid,OC_Table.length,OC_Table.%csubset,OC_Table.%cacc,OC_Table.%cpvalue_v, OC_Table.%cpvalue_e, OC_Table.%cpercentid, Subset_Table.seqtype, BLAST_Run_Table.blast_extension, OC_Table.nsubset,OC_Table.psubset,OC_Table.pubmask ", seqstr[stype], seqstr[stype], seqstr[stype], seqstr[stype], seqstr[stype]);
			dbfcmd(dbproc,"from OC_Table, Subset_Table,BLAST_Run_Table where OC_Table.valid>0 and OC_Table.id>0 and OC_Table.%cpvalue_e<%d and OC_Table.%cpvalue_e>%d and Subset_Table.name=OC_Table.%csubset and BLAST_Run_Table.id=Subset_Table.blastrun_id and OC_Table.%cpercentid>%d and ",
				seqstr[stype], evalmax,
				seqstr[stype], evalmin,
				seqstr[stype],
				seqstr[stype], percmin);

			dbfcmd(dbproc," OC_Table.%cpercentid<%d",
				seqstr[stype],percmax);
			if (!stype && onlyprotein)
			{
				dbfcmd(dbproc,"and OC_Table.pubmask=8 ");
				strcat(querycmd, "and OC_Table.pubmask=8 ");
			}
			for (i=0; i<count; i++)
                  	{
                          if (strcmp(entries[i].name,"SUBSET")==0)
                          {
			     if (nbsubsets==0)
			     {
				strcat(querycmd," and (");
                                dbfcmd(dbproc," and (");
			     }
			     else
                             if (nbsubsets>0)
			     {
				strcat(querycmd," or ");
                                dbfcmd(dbproc," or ");
			     }
			     sprintf(querycmd,"%s OC_Table.%csubset='",querycmd,seqstr[stype]);
			     strcat(querycmd,entries[i].val);
			     strcat(querycmd,"' ");
			     dbfcmd(dbproc, "OC_Table.%csubset='%s' ", seqstr[stype],entries[i].val);
			     nbsubsets++;
			  }
			}
			if (nbsubsets>0)
			{
				strcat(querycmd,")");
				dbfcmd(dbproc,") ");
			}
				
		}
		else
		{
		  nbsubsets=0;
		  for (i=0; i<count; i++)
		  {
		    if (strcmp(entries[i].name,"SUBSET")==0)
		    {
		        if (nbsubsets>0)
			{
			   dbfcmd(dbproc," union ");
			   strcat(querycmd," union ");
			}
			dbfcmd(dbproc,"select OC_Table.id, OC_Table.seedid, OC_Table.length, subset='%s',accession, pvalue_v, pvalue_e, percentid,seqtype, BLAST_Run_Table.blast_extension,OC_Table.nsubset,OC_Table.psubset,OC_Table.pubmask from %s_Table,OC_Table,Subset_Table,BLAST_Run_Table ", entries[i].val, entries[i].val);
			strcat(querycmd,"select OC_Table.id, OC_Table.seedid, OC_Table.length, subset='");
			strcat(querycmd,entries[i].val);
			strcat(querycmd,"', accession, pvalue_v, pvalue_e, percentid,seqtype,BLAST_Run_Table.blast_extension,nsubset,psubset from ");
			strcat(querycmd,entries[i].val);

			strcat(querycmd,"_Table,OC_Table,Subset_Table,BLAST_Run_Table where OC_Table.seedid=");
			dbfcmd(dbproc, " where percentid>%d and percentid<=%d and pvalue_e>=%d and pvalue_e<%d and OC_Table.seedid=%s_Table.seedid and OC_Table.valid>0 and OC_Table.id>0 and Subset_Table.name='%s' and BLAST_Run_Table.id=Subset_Table.blastrun_id ",
				percmin, percmax,
				evalmin, evalmax,
				entries[i].val,entries[i].val);
			strcat(querycmd,entries[i].val);
			strcat(querycmd,"_Table.seedid and OC_Table.valid>0 and OC_Table.id>0 and Subset_Table.name='");
			strcat(querycmd, entries[i].val);
			strcat(querycmd,"' and BLAST_Run_Table.id=Subset_Table.blastrun_id");
			nbsubsets++;
		    }
	  	  }
		}
		if ((nbsubsets==0)&&(!onlybesthit))
                {
                   dbproc2=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_PROD_SERVER,"PFAM_DB");
                   nbshown=(long)print_unknown_cluster_list(dbproc,dbproc2,totalclusters,triggerat,queryorder, hidemasked,forcepfam,nbignore);
                }
		else
		{
		   dbfcmd(dbproc," %s", queryorder);
		   strcat(querycmd," ");
		   strcat(querycmd,queryorder);
		   nbshown=(long)display_cluster_list(dbproc, totalclusters,maxhitpercluster,triggerat,nbignore,NULL);
		}
	/*
		printf("<INPUT TYPE=HIDDEN NAME=NBIGNORE VALUE=%d></FORM>\n",
			nbignore+nbshown,kwq); */
#if OUTPUT_QUERY
		printf("QUERYCMD<BR>[%s]<BR>\n", querycmd);
#endif
		printf("</BODY></HTML>");
		exit(0);
		break;
    case 34:
		sprintf(strtmp,"Lexicon ASAP - Available Tissues from Expression Panel of LexGenes");
		print_cleartop(1,0,0,strtmp,"white","black");
		printf("<CENTER>%s<FONT COLOR=white>%s</FONT></FONT></CENTER>",
				TIMES_14,strtmp);
		already=0;
		ti=(DB_TISSUE *)lexdb_malloc_tissue();
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		dbcmd(dbproc,"select id,name, species,description from Tissue_Table");
		dbsqlexec(dbproc);
		while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS)
		{
		    if (rt==SUCCEED)
		    {
			dbbind(dbproc,1,INTBIND,(DBINT)0,(BYTE *)&(ti->id));
			dbbind(dbproc,2,STRINGBIND,(DBINT)0,
					(BYTE DBFAR *)(ti->name));
			dbbind(dbproc,3,STRINGBIND,(DBINT)0,
					(BYTE DBFAR *)(ti->species));
			dbbind(dbproc,4,STRINGBIND,(DBINT)0,
					(BYTE DBFAR *)(ti->description));
			while (dbnextrow(dbproc)!=NO_MORE_ROWS)
			{
			  if (!already)
			  {
				already=1;
				printf("<TABLE BORDER=0 WIDTH=100%%>");
				printf("<TR>");
				printf("<TH BGCOLOR=#209090 WIDTH=15>%s<FONT COLOR=white>#</FONT></FONT></TH>", TIMES_10);
				printf("<TH BGCOLOR=#209090 WIDTH=100>%s<FONT COLOR=white>Tissue Name</FONT></FONT></TH>", TIMES_10);
				printf("<TH BGCOLOR=#209090 WIDTH=100>%s<FONT COLOR=white>Species</FONT></FONT></TH>", TIMES_10);
				printf("<TH BGCOLOR=#209090>%s<FONT COLOR=white>Description</FONT></FONT></TH>", TIMES_10);
				printf("</TR>");
		   	  }
			  printf("<TR>");
			  printf("<TH BGCOLOR=white>%s<FONT COLOR=black>%ld</FONT></FONT></TH>", TIMES_10,ti->id);
			  printf("<TH BGCOLOR=white>%s<FONT COLOR=black>%s</FONT></FONT></TH>", TIMES_10,ti->name);
			  printf("<TH BGCOLOR=white>%s<FONT COLOR=black>%s</FONT></FONT></TH>", TIMES_10,ti->species);
			  if (strlen(description)<3)
			     printf("<TD BGCOLOR=white ALIGN=TOP>%s<FONT COLOR=black>no description</FONT></FONT></TD>", TIMES_10);
			  else
			     printf("<TD BGCOLOR=white ALIGN=TOP>%s<FONT COLOR=black>%s</FONT></FONT></TD>", TIMES_10,ti->description);
			  printf("</TR>");
			  
			}
		    }
		}
		if (already) printf("</TABLE>");
		else
			printf("<P><BR>%s<FONT COLOR=white>There are no defined Tissue in the Expression Panel</FONT></FONT>",TIMES_10);
		
		dbexit();
		if (ti!=NULL) free(ti);
		exit(0);
		break;
    case 35:
		if (sscanf(entries[0].val,"%ld",&plateid)!=1)
		   print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Plate Number");
		if (plateid<0)
		{
		   sprintf(strtmp,"Lexicon ASAP - Tagging Plate %s%ld as NOT Ready to be processed",DB_PLATE_PREFIX,plateid);
		}
		else
		{
		   sprintf(strtmp,"Lexicon ASAP - Tagging Plate %s%ld as Ready to be processed",DB_PLATE_PREFIX,plateid);
		}
		print_cleartop(1,0,0,strtmp,"white","black");
		Sometime=time(&Sometime);
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		if (plateid<0)
		   dbfcmd(dbproc,"update Plate_Table set pdate=0 where id=%ld and processed=0 and pdate>0 and valid=1", -plateid);
		else
		   dbfcmd(dbproc,"update Plate_Table set pdate=%ld where id=%ld and processed=0 and pdate=0 and valid=1", Sometime, plateid);
	        dbsqlexec(dbproc);
		while (dbresults(dbproc)!=NO_MORE_RESULTS) continue;
		if (plateid>0)
			printf("<CENTER>%s<FONT COLOR=white>Plate %s%ld has been marked for Processing</FONT></FONT></CENTER>",TIMES_10,DB_PLATE_PREFIX,plateid);
		else
			printf("<CENTER>%s<FONT COLOR=white>Plate %s%ld has been unmarked for Processing</FONT></FONT></CENTER>",TIMES_10,DB_PLATE_PREFIX,-plateid);
		dbexit();
		exit(0);
		break;
    case 36:
		if (strcpy(strtmp,entries[0].val)==NULL)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid GeneSeq Accession Number");
		sprintf(strtmp,"GeneSeq (Patent Office Database) Information for %s", entries[0].val);
		print_cleartop(1,0,0,strtmp,"black","white");
		result_p =(char *) getRecordBySubsetAcc("gs", entries[0].val);
		if (result_p==NULL)
		{
			printf("Could not find Accession [%s] in gsp",entries[0].val);
		}
		else
			printf("<PRE>%s%s</FONT></PRE>",COURIER_10,result_p);
		free(&result_p);
		exit(0);
		break;
    case 37:
		sprintf(strtmp,"Lexicon ASAP - Database Production Options for %s", DB_HTML_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
		printf("<CENTER>%s<FONT COLOR=white><BR><B>%s DB Production Options</B></FONT></FONT><BR>",TIMES_14,DB_HTML_NAME);
		printf("%s<FONT COLOR=white><I>Please choose one of the following options</I></FONT></FONT><P></CENTER>",TIMES_10);
		make_sub_cell("white","white","#209090");
		printf("%s<FONT COLOR=black><P ALIGN=JUSTIFY>Before creating Plates to Import into the %s Database, you must create and define the <I>Protocols</I> used in sequencing and/or tissue culture. Doing so will later allow the production of comparative statistics between these Protocols at any production or analysis level.</P>",TIMES_10,DB_HTML_NAME);
		close_cell();
		printf("<TABLE BORDER=0 WIDTH=100%%>");
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?NEW_PROTOCOL=1','NEW_PROTOCOL','scrollbars=yes,resizable=yes,width=680,height=400,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/createprotocol.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?LIST_PROTOCOLS=0','NEW_PLATE','scrollbars=yes,resizable=yes,width=680,height=340,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/listprotocols.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("</TABLE>");
		printf("<P>");
		make_sub_cell("white","white","#209090");
		printf("%s<FONT COLOR=black><P ALIGN=JUSTIFY>In order to import ABI Files for processing in the %s database, you need first to create a <I>96 Well Plate Diagram<I> resulting from sequencing. You will be proposed to specify which specific well(s) on your plate are control wells which should not be imported. In order to create a New Plate, there must alreadt have defined <I>Protocols</I> (see above). A <U>Processed Plate</U> has already been imported. A <U>Ready Plate</U> has not yet been imported but all ABI Files for it have been spooled. A <U>Pending Plate</U> is an unprocessed Plate for which there are still ABI Files to spool.</P>",TIMES_10,DB_HTML_NAME);
		close_cell();
		printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?NEW_PLATE=1','NEW_PLATE','scrollbars=yes,resizable=yes,width=680,height=340,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/createnewplate.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?LIST_PLATES=0','PLATE_LIST','scrollbars=yes,resizable=yes,width=680,height=480,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/listplates.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?LIST_PLATES=1','PLATE_LIST','scrollbars=yes,resizable=yes,width=680,height=480,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/processedplates.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?LIST_PLATES=2','PLATE_LIST','scrollbars=yes,resizable=yes,width=680,height=480,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/readyplates.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?LIST_PLATES=3','PLATE_LIST','scrollbars=yes,resizable=yes,width=680,height=480,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/pendingplates.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><A HREF=\"#\" onClick=\"window.open('%s?LIST_PLATES=4','PLATE_LIST','scrollbars=yes,resizable=yes,width=680,height=480,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/yourplates.jpg\" BORDER=0></A>", DB_NAME,LEXDB_HTDOCS_DIR);
		printf("</TR></TABLE>");
		printf("<HR WIDTH=100%%>");
		printf("<CENTER>%s<FONT COLOR=white><BR><B>%s DB Production Plot</B></FONT></FONT><BR>",TIMES_14,DB_HTML_NAME);
		printf("<CENTER><IMG SRC=%s/%s/production/production.gif ALT='%s Production Plot (07/13/1999)'>",LEXDB_HTDOCS_DIR,DB_NAME,DB_NAME);
		break;
    case 38:
		sprintf(strtmp,"Lexicon ASAP - Creating a New Protocol for %s", DB_HTML_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
		print_new_protocol(NULL);
		exit(0);
		break;
    case 39:
		sprintf(strtmp,"Lexicon ASAP - Creating a New Plate for %s", DB_HTML_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
	
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		print_new_plate(dbproc);
		dbexit();
		exit(0);
		break;
    case 40:
		sprintf(strtmp,"Lexicon ASAP - Saving New Protocol for %s", DB_HTML_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		save_new_protocol(dbproc);
		exit(0);
		dbexit();
		break;
    case 41:
		sprintf(strtmp,"Lexicon ASAP - List of Existing Protocols for %s", DB_HTML_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		list_protocols(dbproc);
		dbexit();
		exit(0);
		break;
    case 42:
		sprintf(strtmp,"Lexicon ASAP - Creating a new 96-well Plate for %s", DB_HTML_NAME);
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
		save_new_plate(dbproc);
		dbexit();
		exit(0);
		break;
    case 43:
		sprintf(strtmp,"Lexicon ASAP - Listing All 96-well Plates in %s", DB_HTML_NAME);
        	dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
		if (sscanf(entries[0].val,"%d",&i)!=1)
			i=0; /* ALL PLATES BY DEFAULT */

		if (strcmp(entries[1].name,"START_AT")==0)
		{
		   if (sscanf(entries[1].val,"%ld",&j)!=1) j=0;
		}
		list_plates(dbproc,i,j);
		dbexit();
		exit(0);
		break;
    case 44:
		sprintf(strtmp,"Lexicon ASAP - %s Protocol Information",DB_HTML_NAME);
                dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
                print_cleartop(1,0,0,strtmp,"white","black");
                if (sscanf(entries[0].val,"%ld",&i)!=1)
                        i=0;
                show_protocol(dbproc,i);
                dbexit();
                exit(0);
                break;
    case 46:
		Sometime=time(&Sometime);
		sprintf(strtmp,"%s Cluster List Searches Results %s", DB_HTML_NAME,ctime(&Sometime));
		strtmp[strlen(strtmp)-1]='\0';	/* Remove the \n from ctime */
		print_cleartop2(1,0,1,strtmp,"white","black");
		make_sub_cell("black","black","#209090");
		printf("<CENTER>%s<FONT COLOR=white><B>%s</B> ", TIMES_10,strtmp);
		printf("</FONT></TD><TD BGCOLOR=black><CENTER>");
		foundinc=0;
			/* ONE PASS TO READ ALL ENTRIES */
		printf("<FORM METHOD=GET ACTION=%s><INPUT TYPE=HIDDEN NAME=NEW_CLUSTER_LIST VALUE=1>",DB_NAME);
		for (i=0; i<count; i++)
		{
#if VERB
		    printf("%s=%s ",entries[i].name, entries[i].val);
#endif
		    if (strcmp(entries[i].name,"ONLYBESTHIT")==0)
		    {
			onlybesthit=1;
                        printf("<INPUT TYPE=HIDDEN NAME=ONLYBESTHIT VALUE=1>\n");
		    }
		    if (strcmp(entries[i].name,"NBIGNORE")==0)
                    {
                        if (sscanf(entries[i].val,"%ld",&nbignore)!=1)
                                nbignore=0;
                        else
                        {
                                if (nbignore<0) nbignore=0;
                        }
                    }
		    if (strcmp(entries[i].name,"TRIGGER")==0)
                    {
                        if (sscanf(entries[i].val,"%ld", &triggerat)!=1)
                                triggerat= -1;
			if (triggerat<1) triggerat= -1;
#if 0
                        else
                            printf("<INPUT TYPE=HIDDEN NAME=TRIGGER VALUE=%d>\n",triggerat);
#endif
                    }

		    if (strcmp(entries[i].name,"MAXCLUSTERS")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&totalclusters)!=1)
                                totalclusters=25;
                        else
                        {
                           if (totalclusters<1)
                                totalclusters=10;
                           if (totalclusters>100)
                                totalclusters=100;
                        }
			printf("<INPUT TYPE=HIDDEN NAME=MAXCLUSTERS VALUE=%d>", totalclusters);
		    }
#if 0
		    if (strcmp(entries[i].name,"MAXHITPERCLUSTER")==0)
                    {
                        if (sscanf(entries[i].name,"%d",&maxhitpercluster)!=1)
                                maxhitpercluster=1;
                        printf("<INPUT TYPE=HIDDEN NAME=MAXHITPERCLUSTER VALUE=1>\n");
                    }
#endif
		    maxhitpercluster=1;	/* Only one per cluster */
		    if (strcmp(entries[i].name,"KEYWORD")==0)
		    {
			if (strlen(entries[i].val)>2)
			{
			  haskw=1;
			  strcpy(keyword,entries[i].val);
			  strcpy(kwquery,"");
			  for (j=0; j<strlen(entries[i].val); j++)
			  {
			    if (isalpha(entries[i].val[j]))
			    {
			      sprintf(kwquery,"%s[%c%c]",
				kwquery,
				tolower(entries[i].val[j]),
				toupper(entries[i].val[j]));
			    }
			    else
			      sprintf(kwquery,"%s%c",
				kwquery,
				entries[i].val[j]);
			  }
			}
			else
			   strcpy(kwquery,"");
			printf("<INPUT TYPE=HIDDEN NAME=KEYWORD VALUE='%s'>", entries[i].val);
		    }
		    if (strcmp(entries[i].name,"NOABSTRACT")==0)
		    {
			noabstract=1;
		    }
		    if (strcmp(entries[i].name,"Click to Count How Many Clusters Match Your Query")==0)
		    {
			onlycount=1;
		    }
		    if (strcmp(entries[i].name,"PLATES")==0)
		    {
			useplates=1;
			platemode=0;
			strcpy(platestr,entries[i].val);
			switch(entries[i].val[0])
			{
			   case '>': 
				platemode=2;
				break;
			   case '<':
				platemode=1;
				break;
			}
			if (sscanf(entries[i].val+(int)(platemode>0), "%ld",&plate_id)!=1)
			{
			  printf("Problem loading the QUERY BY PLATES value\n");
			  useplates=0;  
			}
			if (useplates)
			{
			  switch(platemode)
			  {
			    case 0:
				if (plate_id>0)
			  	  sprintf(platequery," and plate_id=%ld",
					plate_id);
				else
				{
				   printf("Plate ID must be a positive number (ignoring query by Plate ID)<BR>\n");
				   useplates=0;
				}
				break;
			    case 1:
			  	sprintf(platequery," and plate_id<%ld",
					plate_id);
				break;
			    case 2:
			  	sprintf(platequery," and plate_id>%ld",
					plate_id);
				break;
			  }
			}
			printf("<INPUT TYPE=HIDDEN NAME=PLATES VALUE='%s'>", entries[i].val);
   		    }
		    if (strcmp(entries[i].name,"PROTOCOLS")==0)
                    {
                        useprotocols=1;
                        protomode=0;
			strcpy(protostr,entries[i].val);
                        switch(entries[i].val[0])
                        {
                           case '>':
                                protomode=2;
                                break;
                           case '<':
                                protomode=1;
                                break;
                        }
                        if (sscanf(entries[i].val+(int)(protomode>0), "%ld",&proto_id)!=1)
                        {
                          printf("Problem loading the QUERY BY PROTOCOLS value\n");
                          useprotocols=0;
                        }
                        if (useprotocols)
                        {
                          switch(protomode)
                          {
                            case 0:
                                if (proto_id>0)
                                  sprintf(protoquery," and proto_id=%ld",
                                        proto_id);
                                else
                                {
                                   printf("Protocol ID must be a positive number (ignoring query by Protocol ID)<BR>\n");
                                   useprotocols=0;
                                }
                                break;
                            case 1:
                                sprintf(protoquery," and proto_id<%ld",
                                        proto_id);
                                break;
                            case 2:
                                sprintf(protoquery," and proto_id>%ld",
                                        proto_id);
                                break;
                          }
                        }
			printf("<INPUT TYPE=HIDDEN NAME=PROTOCOLS VALUE='%s'>", entries[i].val);
                    }

		    if (strcmp(entries[i].name,"ORDERBY")==0)
		    {
			if (sscanf(entries[i].val,"%d",&orderby)!=1)
			   orderby=0;
			printf("<INPUT TYPE=HIDDEN NAME=ORDERBY VALUE=%s>", entries[i].val);
		    }
		    if (strcmp(entries[i].name,"MAX_EVALUE_PROT")==0)
		    {
			if (sscanf(entries[i].val,"%d",&maxevalue_prot)!=1)
				maxevalue_prot=-1;
			printf("<INPUT TYPE=HIDDEN NAME=MAX_EVALUE_PROT VALUE=%s>",entries[i].val);
		    }
		    if (strcmp(entries[i].name,"MIN_EVALUE_PROT")==0)
		    {
			if (sscanf(entries[i].val,"%d",&minevalue_prot)!=1)
                                minevalue_prot=-251;
			printf("<INPUT TYPE=HIDDEN NAME=MIN_EVALUE_PROT VALUE=%s>",entries[i].val);
		    }
		    if (strcmp(entries[i].name,"MAX_EVALUE_NUC")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&maxevalue_nuc)!=1)
                                maxevalue_nuc=-1;
			printf("<INPUT TYPE=HIDDEN NAME=MAX_EVALUE_NUC VALUE=%s>",entries[i].val);
                    }
                    if (strcmp(entries[i].name,"MIN_EVALUE_NUC")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&minevalue_nuc)!=1)
                                minevalue_nuc=-251;
			printf("<INPUT TYPE=HIDDEN NAME=MIN_EVALUE_NUC VALUE=%s>",entries[i].val);
                    }
		    if (strcmp(entries[i].name,"MAX_PERCENT_NUC")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&maxpercentid_nuc)!=1)
                                maxpercentid_nuc=100;
			printf("<INPUT TYPE=HIDDEN NAME=MAX_PERCENT_NUC VALUE=%s>",entries[i].val);
                    }
		    if (strcmp(entries[i].name,"MIN_PERCENT_NUC")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&minpercentid_nuc)!=1)
                                minpercentid_nuc=0;
			printf("<INPUT TYPE=HIDDEN NAME=MIN_PERCENT_NUC VALUE=%s>",entries[i].val);
                    }
		    if (strcmp(entries[i].name,"MAX_PERCENT_PROT")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&maxpercentid_prot)!=1)
                                maxpercentid_prot=100;
			printf("<INPUT TYPE=HIDDEN NAME=MAX_PERCENT_PROT VALUE=%s>",entries[i].val);
                    }
                    if (strcmp(entries[i].name,"MIN_PERCENT_PROT")==0)
                    {
                        if (sscanf(entries[i].val,"%d",&minpercentid_prot)!=1)
                                minpercentid_prot=0;
			printf("<INPUT TYPE=HIDDEN NAME=MIN_PERCENT_PROT VALUE=%s>",entries[i].val);
                    }


		    if (strncmp(entries[i].val,"INCLUDE_PROT_",13)==0)
                    {
                        if (sscanf(entries[i].val+13,"%ld",&subsetid)!=1)
                                printf("Can not extract subsetid\n");
			else
			{
#if VERB
                          printf(" -> PROTEIN SUBSET %ld TO INCLUDE !",subsetid);
#endif
                          protmask=update_bitmask((long)protmask, subsetid-1);
			  printf("<INPUT TYPE=HIDDEN NAME=%s VALUE=%s>",entries[i].name,entries[i].val);
                          foundinc=1;
			}
                    }
		    if (strncmp(entries[i].val,"INCLUDE_NUC_",12)==0)
		    {
			if (sscanf(entries[i].val+12,"%ld",&subsetid)!=1)
				printf("Can not extract subsetid\n");
			else
			{
#if VERB
			  printf(" -> NUCLEOTIDE SUBSET %ld TO INCLUDE !",subsetid);
#endif
			  nucmask=update_bitmask((long)nucmask, subsetid-1);
			  printf("<INPUT TYPE=HIDDEN NAME=%s VALUE=%s>",entries[i].name,entries[i].val);
			  foundinc=1;
			}
		    }
		    if (strncmp(entries[i].val,"EXCLUDE_",8)==0)
		    {
			if (sscanf(entries[i].val+8,"%ld",&subsetid)!=1)
				printf("Can not extract subsetid\n");
			else
			{
#if VERB
			  printf(" -> SUBSET %ld TO EXCLUDE !",subsetid);
#endif
			  excmask=update_bitmask((long)excmask, subsetid-1);
			  printf("<INPUT TYPE=HIDDEN NAME=%s VALUE=%s>",entries[i].name,entries[i].val);

			}
		    }
#if VERB
		    printf("<BR>");
#endif
		}
		if (!foundinc)
		{
			printf("</FORM>");
			printf("%s<FONT COLOR=white></B>You must include at least one Subset to include in your search.</FONT></FONT>",TIMES_12);
			close_cell();
			exit(0);
		}
		printf("<INPUT BORDER=0 TYPE=IMAGE NAME=\"Click here to view the next %d Clusters matching your search\" SRC=%s/gifs/asap/next.gif></INPUT>",totalclusters,LEXDB_HTDOCS_DIR);
		close_cell();
                dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
#if 0
		if (onlybesthit && strcmp(DB_USER,"cperson")==0)
#endif
		if (onlybesthit)
		{
		  make_search_query(dbproc, onlybesthit, kwquery, platestr, protostr,orderby, maxevalue_prot, minevalue_prot, maxpercentid_prot, minpercentid_prot, maxevalue_nuc, minevalue_nuc, maxpercentid_nuc, minpercentid_nuc, protmask, nucmask, excmask);
		}
		else
		{
		          /* ONE PASS TO MAKE THE QUERY */
                  for (i=0; i<count; i++)
                  {
		    if (strncmp(entries[i].val,"INCLUDE_PROT_",13)==0)
                    {
			  if (!unionstarted)
			  {
				unionstarted=1;
			  }
			  else
			  {
				sprintf(querycmd,"%s union ",querycmd);
				dbfcmd(dbproc," union ");
			  }
                    	  sprintf(querycmd,"%s select ocid,ostid,oclength,subset='%s',accession, pvalue_v,pvalue_e, percentid,seqtype,blast_extension, nsubset,psubset,spubmask ", querycmd, entries[i].name);
                          dbfcmd(dbproc," select ocid,ostid,oclength,subset='%s',accession, pvalue_v,pvalue_e, percentid,seqtype,blast_extension, nsubset,psubset,spubmask",  entries[i].name);
			  strcat(querycmd," from ");
			  dbfcmd(dbproc," from ");
			  sprintf(querycmd,"%s %s_Search where ",
					querycmd, entries[i].name);
			  dbfcmd(dbproc,"%s_Search where ", entries[i].name);
			
			  if (onlybesthit)
			  {
				sprintf(querycmd,"%s accession=pacc and ",querycmd);
				dbfcmd(dbproc," accession=pacc and ");
			  }
			  /* If no subsets selected, it means any */
			  if (protmask>0)
			  {
				sprintf(querycmd,"%s spubmask&%ld>0 and ",
					querycmd, protmask);
				dbfcmd(dbproc," spubmask&%ld>0 and ",
					 protmask);
 			  }
			  if (excmask>0)
			  {
				sprintf(querycmd,"%s spubmask&%ld=0 and ",
					querycmd, excmask);
				dbfcmd(dbproc," spubmask&%ld=0 and ",
					excmask);
			  }
			  if (haskw)
			  {
				dbfcmd(dbproc, " definition like '%%%s%%' and ", kwquery);
				sprintf(querycmd, "%s definition like '%%%s%%' and ", querycmd,kwquery);
 			  }
			  sprintf(querycmd,"%s pvalue_e<%d and pvalue_e>%d and percentid<%d and percentid>%d ", querycmd, maxevalue_prot, minevalue_prot, maxpercentid_prot, minpercentid_prot);
			  dbfcmd(dbproc," pvalue_e<%d and pvalue_e>%d and percentid<%d and percentid>%d ",  maxevalue_prot, minevalue_prot, maxpercentid_prot, minpercentid_prot);
			  if (useplates)
				sprintf(querycmd,"%s %s", querycmd, platequery);
				dbfcmd(dbproc," %s ",platequery);
			  if (useprotocols)
			  {
				sprintf(querycmd,"%s %s", querycmd, protoquery);
				dbfcmd(dbproc," %s ",protoquery);
			  }
                    }
                    if (strncmp(entries[i].val,"INCLUDE_NUC_",12)==0)
                    {
			  if (!unionstarted)
                          {
                                unionstarted=1;
                          }
                          else
			  {
				sprintf(querycmd,"%s union ",querycmd);
				dbfcmd(dbproc," union ");
			  }
			  sprintf(querycmd,"%s select ocid,ostid,oclength,subset='%s',accession, pvalue_v,pvalue_e, percentid,seqtype,blast_extension, nsubset,psubset,spubmask ", querycmd, entries[i].name);
                          dbfcmd(dbproc," select ocid,ostid,oclength,subset='%s',accession, pvalue_v,pvalue_e, percentid,seqtype,blast_extension, nsubset,psubset,spubmask ",  entries[i].name);
			  strcat(querycmd," from ");
                          dbfcmd(dbproc," from ");
                          sprintf(querycmd,"%s %s_Search where ",
                                        querycmd, entries[i].name);
                          dbfcmd(dbproc,"%s_Search where ", entries[i].name);
                          /* If no subsets selected, it means any */
			  if (onlybesthit)
			  {
				sprintf(querycmd,"%s accession=nacc and ",querycmd);
                                dbfcmd(dbproc," accession=nacc and ");
			  }
                          if (nucmask>0)
                          {
                                sprintf(querycmd,"%s spubmask&%ld>0 and ",
                                        querycmd, nucmask);
                                dbfcmd(dbproc," spubmask&%ld>0 and ",
                                        nucmask);
                          }
                          if (excmask>0)
                          {
                                sprintf(querycmd,"%s spubmask&%ld=0 and ",
                                        querycmd, excmask);
                                dbfcmd(dbproc," spubmask&%ld=0 and ",
                                        excmask);
                          }
			  if (haskw)
			  {
				sprintf(querycmd, "%s definition like '%%%s%%' and ", querycmd,kwquery);
				dbfcmd(dbproc, " definition like '%%%s%%' and ", kwquery);
 			  }
                          sprintf(querycmd,"%s pvalue_e<%d and pvalue_e>%d and percentid<%d and percentid>%d", querycmd,  maxevalue_nuc, minevalue_nuc, maxpercentid_nuc, minpercentid_nuc);
                          dbfcmd(dbproc," pvalue_e<%d and pvalue_e>%d and percentid<%d and percentid>%d",  maxevalue_nuc, minevalue_nuc, maxpercentid_nuc, minpercentid_nuc);
                          if (useplates)
			  {
                                sprintf(querycmd,"%s %s", querycmd, platequery);
				dbfcmd(dbproc," %s ",platequery);
			  }
                          if (useprotocols)
			  {
                                sprintf(querycmd,"%s %s", querycmd, protoquery);
				dbfcmd(dbproc," %s ",protoquery);
			  }
                    }
		  }
#if 1
		  switch(orderby)
                  {
                           case 0:
                                sprintf(querycmd,"%s order by id asc",querycmd);
                                dbfcmd(dbproc," order by ocid asc");
                                break;
                           case 1:
                                sprintf(querycmd,"%s order by id desc",querycmd);
                                dbfcmd(dbproc," order by ocid desc");
                                break;
                           case 2:
                           case 3:
                                printf("ORDER BY PVALUE unavailable yet");
                                break;
                           case 4:
                                sprintf(querycmd,"%s order by oclength desc",querycmd);
                                dbfcmd(dbproc," order by oclength desc");
                                break;

                  }
  		}
#endif

#if 0
		printf("[%s]<BR>\n",querycmd);
#endif
		nbshown=(long)display_cluster_list(dbproc, totalclusters,maxhitpercluster,triggerat,nbignore,keyword);
		report_cluster_legend(TIMES_10);
#if 0
		printf("NUCMASK=%ld (0x%x)<BR>PROTMASK=%ld (0x%x)<BR>EXCMASK=%ld (0x%x)<BR>",nucmask,nucmask, protmask,protmask, excmask,excmask);
#endif
		dbexit();
		break;
    case 47:    /* ACTIVETOP */
		printf("Content-Type: text/html\n\r\n\r");
		printf("<HTML><HEAD><TITLE>Lexicon ASAP! (c)1998,99 Lexicon Genetics Inc.</TITLE><META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\"></HEAD>\n<NOSCRIPT>The ASAP Web Pages uses Javascript functionality. Please enable Javascript in your browser Preferences or contact your system administrator for further help.</P><P ALIGN=JUSTIFY>You will not be able to use all the software features until it is Enabled.</P></NOSCRIPT>",LEXDB_HTDOCS_DIR);
		printf("<BODY BGCOLOR=black TEXT=white><TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=black><FORM METHOD=GET ACTION=%s TARGET=STATSWIN><INPUT TYPE=IMAGE SRC=\"%s/gifs/asap/dbinformation.jpg\" NAME=\"Click to view the Database Contents and Dataflow Description\" BORDER=0></FORM></TH>",DB_NAME,LEXDB_HTDOCS_DIR);
		if (DB_HAS_PRODUCTION)
			printf("<TH BGCOLOR=black><FORM METHOD=GET ACTION=%s TARGET=MAINFRAME><INPUT TYPE=IMAGE SRC=\"%s/gifs/asap/production.jpg\" NAME=\"Click to access the ASAP Production options\" BORDER=0></FORM></TH>",DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black VALIGN=TOP><A HREF=\"#\" onClick=\"window.open('%s?NEWCLUSTER_LIST_PAGE=1','%s_CLUSTER_LIST_PAGE','scrollbars=yes,resizable=yes,width=800,height=650,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/advancedsearches.gif\" ALT=\"Click To Open the NEW Cluster List Panel Window\" BORDER=0></FONT></A></FORM></TH>",DB_NAME,DB_HTML_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><FORM METHOD=GET ACTION=smartblast TARGET=%s_SEQFRAME><INPUT TYPE=IMAGE SRC=\"%s/gifs/asap/sequencesearch.jpg\" NAME=\"Go to the Sequence Searches Page\" BORDER=0></FORM></TH>",DB_NAME,LEXDB_HTDOCS_DIR);
	        if (strlen(LEXDB_SYBASE_PFAMDB_SERVER)>0)
		   printf("<TH BGCOLOR=black VALIGN=TOP><A HREF=\"#\" onClick=\"window.open('%s?PFAM_SEARCH_PAGE=1','%s_PFAM_SEARCH_PAGE','scrollbars=yes,resizable=yes,width=400,height=200,top=10,left=10');\"><IMG SRC=\"%s/gifs/asap/pfamresults.jpg\" ALT=\"Click To Open the PFAM hits Search Form\" BORDER=0></FONT></A></FORM></TH>",DB_NAME,DB_HTML_NAME,LEXDB_HTDOCS_DIR);
		if (strcmp(DB_USER,"mgonzal")==0)
		   printf("<TH BGCOLOR=black><FORM METHOD=GET ACTION=smartblast2 TARGET=%s_SEQFRAME2><INPUT TYPE=IMAGE SRC=\"%s/gifs/asap/sequencesearch.jpg\" NAME=\"Manuel Only Test\" BORDER=0></FORM></TH>",DB_NAME,LEXDB_HTDOCS_DIR);
		printf("<TH BGCOLOR=black><FORM METHOD=GET ACTION=%s TARGET=ONLINEHELP><INPUT TYPE=HIDDEN NAME=ONLINEHELP><INPUT TYPE=IMAGE SRC=\"%s/gifs/asap/onlinehelp.jpg\" NAME=\"Open the ASAP Online Documentation\" BORDER=0></FORM></TH>",DB_NAME,LEXDB_HTDOCS_DIR);
		printf("</TR></TABLE></BODY></HTML>");
		break;
    case 48:    /* ACTIVEBOT */
		printf("Content-Type: text/html\n\r\n\r");
		printf("<HTML><HEAD><TITLE>Lexicon ASAP! (c)1998,99 Lexicon Genetics Inc.[%s Database]</TITLE><META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\"></HEAD><NOSCRIPT>The ASAP Web Pages uses Javascript functionality. Please enable Javascript in your browser Preferences or contact your system administrator for further help.</P><P ALIGN=JUSTIFY>You will not be able to use all the software features until it is Enabled.</P></NOSCRIPT>",DB_HTML_NAME,LEXDB_HTDOCS_DIR);
#if 0
		printf("<SCRIPT LANGUAGE=\"JavaScript\">\n<!--\nif (top == self) self.location.href = \"%s/noframe.html\";\n// -->\n</SCRIPT>\n",LEXDB_HTDOCS_DIR);
#endif
		printf("<BODY BGCOLOR=black TEXT=white>");
     		printf("<TABLE BORDER=0 WIDTH=100%% CELLSPACING=1 CELLPADDING=0><TR>");
     		printf("<FORM METHOD=GET ACTION=%s TARGET=MAINFRAME><TD BGCOLOR=black ALIGN=RIGHT><IMG SRC=%s/gifs/asap/showsequence.jpg ALT=\"Enter a Sequence Tag ID and Press Return\"></TD>",DB_NAME,LEXDB_HTDOCS_DIR);
     		printf("<TH BGCOLOR=black ALIGN=TOP><INPUT TYPE=TEXT NAME=SHOW_SEQUENCE VALUE='' SIZE=8 ALIGN=LEFT></FONT></TH></FORM>");
     		printf("<FORM METHOD=GET ACTION=%s TARGET=MAINFRAME><TD BGCOLOR=black ALIGN=RIGHT><IMG SRC=%s/gifs/asap/showcluster.jpg ALT=\"Enter a Sequence Cluster ID and Press Return\"></TD>",DB_NAME,LEXDB_HTDOCS_DIR);
     		printf("<TH BGCOLOR=black ALIGN=TOP><INPUT TYPE=TEXT NAME=SHOW_CLUSTER VALUE='' SIZE=8 ALIGN=LEFT></FONT></TH></FORM>");
		if (DB_GENOMIC_DNA)
		{
     			printf("<FORM METHOD=GET ACTION=%s TARGET=MAINFRAME><TD BGCOLOR=black ALIGN=RIGHT><IMG SRC=%s/gifs/asap/accession.jpg ALT=\"Enter an Original Accession Number for a Sequence and Press Return\"></TD>",DB_NAME,LEXDB_HTDOCS_DIR);
     			printf("<TH BGCOLOR=black ALIGN=TOP><INPUT TYPE=TEXT NAME=FIND_ACCESSION VALUE='' SIZE=8 ALIGN=LEFT></FONT></TH></FORM>");
		}
		else
		if (DB_HAS_PRODUCTION)
		{
     			printf("<FORM METHOD=GET ACTION=%s TARGET=MAINFRAME><TD BGCOLOR=black ALIGN=RIGHT><IMG SRC=%s/gifs/asap/plateid.jpg ALT=\"Enter a Plate ID and Press Return\"></TD>",DB_NAME,LEXDB_HTDOCS_DIR);
     			printf("<TH BGCOLOR=black ALIGN=TOP><INPUT TYPE=TEXT NAME=DISPLAY_PLATE_DIAGRAM VALUE='' SIZE=8 ALIGN=LEFT></FONT></TH></FORM>");
     			printf("<FORM METHOD=GET ACTION=%s TARGET=MAINFRAME><TD BGCOLOR=black ALIGN=RIGHT><IMG SRC=%s/gifs/asap/platename.jpg ALT=\"Enter a Plate Name and Press Return\"></TD>",DB_NAME,LEXDB_HTDOCS_DIR);
     			printf("<TH BGCOLOR=black ALIGN=TOP><INPUT TYPE=TEXT NAME=DISPLAY_PLATE_DIAGRAM_BY_NAME VALUE='' SIZE=8 ALIGN=LEFT></FONT></TH></FORM>");
		}
		printf("</TR></TABLE></BODY></HTML>");
		exit(0);
		break;
    case 49:
		sprintf(strtmp,"Lexicon ASAP Online Documentation - Copyright(c)1999 Lexicon Genetics Inc.                                ");
		if (sscanf(entries[i].val,"%ld",&i)!=1)
                        print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Help Documentation Index");
		
		print_cleartop2(1,0,1,strtmp,"black","white");
		printf("%s<FONT COLOR=black><U><B>",TIMES_12);
		switch(i)
		{
		  case 0:	/* MAIN PAGE */
		    printf("Welcome to the %s Database Home Page & Lexicon ASAP Web Interface",DB_HTML_NAME);
		    printf("</B></U></FONT></FONT><P>");
		    printf("%s<FONT COLOR=black>",TIMES_10);
		    curfig=1;
		    printf("<P ALIGN=JUSTIFY>From the main page, you can access the following options:</P>");
		    make_sub_cell("white","black","#209090");
		    printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
		    printf("<TD BGCOLOR=black><IMG SRC=%s/gifs/asap/dbinformation.jpg BORDER=0></TD>",LEXDB_HTDOCS_DIR);
		    printf("<TD BGCOLOR=white>%s<P ALIGN=JUSTIFY>Click on this button to read about every <I>Database Subsets</I> used during the automated analysis of sequence tags imported into the database. This page also indicates how they are each used and what analysis programs and arguments were used.</P></TD>",TIMES_10);
		    printf("</TR>");
                    printf("<TR>");
                    printf("<TD BGCOLOR=black><IMG SRC=%s/gifs/asap/advancedsearches.gif BORDER=0></TD>",LEXDB_HTDOCS_DIR);
                    printf("<TD BGCOLOR=white>%s<P ALIGN=JUSTIFY>The ASAP Interface now provides an <I>Advanced Searches</I> form that will allow you to quickly produce a list of <I>Clusters</I> of interest.</P></TD>", TIMES_10);
                    printf("</TR>");
                    printf("<TR>");
                    printf("<TD BGCOLOR=black><IMG SRC=%s/gifs/asap/sequencesearch.jpg BORDER=0></TD>",LEXDB_HTDOCS_DIR);
                    printf("<TD BGCOLOR=white>%s<P ALIGN=JUSTIFY></P>Using this new program allows one to submit a sequence or an GenBank accession number which will be used as initial query to iterated BLASTN searches against some public databases. Successive query sequences are iteratively extended from available sequence information provided through sequences from public databases.</P>", TIMES_10);
		    printf("<P ALIGN=JUSTIFY>Once a final query sequence can not be extended anymore, a multiple sequence alignment is constructed from all significant fragments and an \"autocorrected\" consensus sequence is created (only if one species was used). The final BLASTN output file for this consensus against all hits is also available in regular HTML or graphic output.</P>");
                    printf("</TR></TABLE>");
                    close_cell(); 
		    break;
		  case 1:
		    printf("How to use the Clusters List Search Panel?");
		    printf("</B></U></FONT></FONT>");
		    printf("%s<FONT COLOR=black>",TIMES_10);
		    printf("<P ALIGN=JUSTIFY> There are several ways to obtain a list of clusters from the %s Database depending on your species or gene of interest. This document describes each of the search options available and how to use them efficiently.</P>",DB_HTML_NAME);
		    printf("<P ALIGN=JUSTIFY>Each cluster consists of a consensus sequence contructed from a set of individual sequences that overlap with each other. Sequence Homology Searches are performed against each <I>Cluster Sequence</I> against several subsets organized by sequence type and by species when applicable. The list of such <I>Subsets</I> appears twice in the <I>Cluster List Search Panel</I>.</P>");
		    curfig=1;
		    printf("<IMG SRC=%s/gifs/asap/help1a.jpg><BR>",LEXDB_HTDOCS_DIR);
		    printf("%s<U>Figure %d:</U> List Order and Search to Each Cluster Best Hit Only.</FONT>",HELV_10,curfig);
		    printf("<P ALIGN=JUSTIFY>Figure %d shows how to choose a preferred ordering of Clusters. The currently available options include: \"By Cluster ID ascending\", \"By Cluster ID descending\" and \"By Cluster Length descending\".</P>",curfig);
		    printf("<P ALIGN=JUSTIFY>If the \"Best Cluster Hits Only\" box is checked, only the most significant hits for each cluster, if any, will be searched and listed. This option should be used most of the time since it allows much faster searches. When not checked, all clusters and their hits will be combined in the searches, according to your subset selection.</P>");
		    printf("<IMG SRC=%s/gifs/asap/help1b.jpg><BR>",LEXDB_HTDOCS_DIR);
		    curfig++;
		    printf("%s<U>Figure %d:</U> Searching by Keyword</FONT>",HELV_10,curfig);
		    printf("<P ALIGN=JUSTIFY>Figure %d shows the box to use in order to restrict the list of Clusters to the ones with hits whose Definition or Description includes one or more specific keywords of your choice. The minimum number of character to specify as keyword search is 3.</P>",curfig);
		    curfig++;
		    if (DB_HAS_PRODUCTION)
		    {
			printf("<IMG SRC=%s/gifs/asap/help1c.jpg><BR>",LEXDB_HTDOCS_DIR);
			printf("%s<U>Figure %d:</U> Searching by Plate ID or Protocol ID</FONT>",HELV_10,curfig);
			printf("<P ALIGN=JUSTIFY>To restrict your cluster list to specific production information, like ASAP Plate Numbers or Protocol ID, please use the Input boxes as indicated on Figure %d. There are 3 different ways to specify which plates (or protocols) one wants to use: an exact plate number, a number preceded by a greater than OR smaller than sign.</P>",curfig);
			curfig++;
		    }
		    printf("<IMG SRC=%s/gifs/asap/help1d.jpg><BR>",LEXDB_HTDOCS_DIR);
		    printf("%s<U>Figure %d:</U> Listing Clusters according to their hits into \"Sequence Subsets\".</FONT>",HELV_10,curfig);
		    printf("<P ALIGN=JUSTIFY>The first listing of all available subsets is used to restrict the list of clusters HAVING at least one homology hit in them. At least one subset must be selected in order to performed a search. For example, if one is interested into hits from the ratRNA database, the ratRNA box should be checked. If the \"Best Cluster Hit Only\" is checked also, only the cluster whose BEST HIT is a sequence from the ratRNA subset will be displayed.</P>");
		    printf("<P ALIGN=JUSTIFY>It is faster to search the \"Best Cluster Hits\" than all hits in the database. When a displayed cluster and its requested hit is in fact the Best Hit in the cluster, it is indicated by a <IMG SRC=%s/gifs/asap/best.gif> icon.</P>",LEXDB_HTDOCS_DIR);
		    printf("<P ALIGN=JUSTIFY>Selecting several Subsets in this <I>Subset List to Include</I> will result in the report of any cluster with at least one hit from any one of these subsets.  When selecting protein and nucleotide subsets altogether in the same query, protein hits tends to be reported more often since they are often more informative.</P>");
		    printf("<P ALIGN=JUSTIFY>You can also specify Evalue or Percentage of Identity cutoffs and only clusters (and their hits) matching your parameters will be displayed. 2 different sets of cutoff can be used for Protein or Nucleotides subsets.</P>");
		    printf("<P ALIGN=JUSTIFY>In general, avoid selecting more than 10 subsets in each Select window. The search response time is directly linked to the number of subsets you decide to include in your search.</P>");
		    curfig++;
		    printf("<IMG SRC=%s/gifs/asap/help1e.jpg><BR>",LEXDB_HTDOCS_DIR);
		    printf("%s<U>Figure %d:</U> Excluding Clusters from the list according to their hits into \"Sequence Subsets\".</FONT>",HELV_10,curfig);
		    printf("<P ALIGN=JUSTIFY>If you check any subset in the Exclude Window, all Clusters with hits to sequence(s) from this subset will NOT BE REPORTED. Often, adding EST hits to the search do not provide additional annotation information but due to nowadays large amount of ESTs in the public database, it may increase your search response time.</P>");
		    curfig++;
		    printf("<IMG SRC=%s/gifs/asap/help1f.jpg><BR>",LEXDB_HTDOCS_DIR);
		    printf("%s<U>Figure %d:</U> The Cluster List.</FONT>",HELV_10,curfig);

		    printf("<P ALIGN=JUSTIFY>The results from your search will be displayed in a list as shown Figure %d. Only the best hit, according to your search, will be reported. The first column indicate a Cluster Number and an Sequence Number that matched your search parameters. If you move your mouse pointer over the <IMG SRC=%s/gifs/asap/brownball.jpg WIDTH=10 HEIGHT=10> icon, a subwindow will appear that will display the list of all subsets a particular cluster sequence had \"hits\" to.</P>",curfig,LEXDB_HTDOCS_DIR);
		    printf("<P ALIGN=JUSTIFY>One column will display an <I>Accession Number</I>, often an hyperlink to some external repository database (ncbi, etc). Between parenthesis appear the name of the subset the reported sequence belongs to. <U>If you move your mouse pointer over the colored balls representing a sequence type (protein, RNA, EST or Nucleotide), the pairwise alignment between this sequence and the cluster will be dynamically displayed in the same page.\n</P>");
		    printf("<P ALIGN=JUSTIFY>The definition provided for this sequence is reported in the Definition column, and the corresponding Evalue and Percentage of Identity for this hit are reported in the most right column.</P>");

		    break;
		  case 2:
		    printf("Extended BLAST Searches & FASTAlign Web Interface");
		    printf("</B></U></FONT></FONT>");
		    printf("%s<FONT COLOR=black>",TIMES_10);
		    printf("<P ALIGN=JUSTIFY>Under development... Some help is available from the interface itself.</P>");
		    break;
		  case 3:
		    printf("");
		    printf("</B></U></FONT></FONT>");
		    printf("%s<FONT COLOR=black>",TIMES_10);
		    printf("<P ALIGN=JUSTIFY>Copyright, Trademarks & References.</P>");
		}
		    break;

		printf("</FONT></FONT>");
		exit(0);
		break;
    case 50:
		sprintf(strtmp,"ASAP Online Documentation - Copyright(c)1999 Lexicon Genetics Inc.", DB_HTML_NAME,ctime(&Sometime));
		print_cleartop2(1,0,1,strtmp,"black","white");
		printf("<CENTER>%s<FONT COLOR=#209090><U><B>Lexicon Automated Sequence Analysis Package</FONT></FONT><BR>%sOnline Documentation</B></U></FONT></CENTER>",TIMES_18,TIMES_14);
		printf("<P>");
		printf("<A HREF=\"#\" onClick=\"window.open('%s?HELPWIN=0','HELPWIN', 'scrollbars=yes,resizable=yes,width=700,height=400,top=10,left=10');\"><FONT COLOR=blue>Main Page</FONT></A>",DB_NAME);
		printf("<P>");
		printf("<A HREF=\"#\" onClick=\"window.open('%s?HELPWIN=1','HELPWIN', 'scrollbars=yes,resizable=yes,width=700,height=400,top=10,left=10');\"><FONT COLOR=blue>Advanced Cluster List Searches</FONT></A>",DB_NAME);
		printf("<P>");
		printf("<A HREF=\"#\" onClick=\"window.open('%s?HELPWIN=2','HELPWIN', 'scrollbars=yes,resizable=yes,width=700,height=400,top=10,left=10');\"><FONT COLOR=blue>Extended BLAST Searches and FASTAlign Web Interface</FONT></A>",DB_NAME);
		printf("<P>");
		printf("<A HREF=\"#\" onClick=\"window.open('%s?HELPWIN=3','HELPWIN', 'scrollbars=yes,resizable=yes,width=700,height=400,top=10,left=10');\"><FONT COLOR=blue>Copyright, Trademarks & References</FONT></A>",DB_NAME);
		break;

    case 51:
		if (sscanf(entries[0].val,"%ld",&i)!=1)
		   print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Query");
		sprintf(strtmp,"%s/databases/%s/users/%c/%s/message.%ld",LEXDB_ROOT_DIR,DB_NAME,tolower(DB_USER[0]),DB_USER,i);
		if ((fd=fopen(strtmp,"w"))==NULL) fclose(fd);
		printf("Content-Type: text/html\r\n\r\n");
                printf("<HEAD><META HTTP-EQUIV=\"Refresh\" content=\"0;URL=%s\"</HEAD>",DB_NAME);
                printf("<BODY BGCOLOR=black></BODY></HTML>");
		exit(0);
    case 52:
		if (strcmp(DB_USER,"cperson")!=0)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Higher Access Level required to perform this operation");
		if (sscanf(entries[0].val,"%ld",&i)!=1)
		{
			sprintf(strtmp,"Incorrect Cluster Number [%s]\n",entries[0].val);
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,strtmp);
		}
		
		print_cleartop2(1,0,1,strtmp,"white","black");
		sprintf(strtmp,"%s/databases/%s/locks/reblast_cluster.lock",
			LEXDB_ROOT_DIR,DB_NAME);
		if ((fd=fopen(strtmp,"r"))!=NULL)
		{
			printf("There is already a ReBLAST Cluster Program running",DB_CPREFIX,i);
		}
		else
		{
			sprintf(strtmp,"%s/bin/lex_reblast_cluster_replace %s %ld",
				LEXDB_ROOT_DIR, DB_NAME,i);
			printf("Reblast Cluster Command Launched for %s%ld",DB_CPREFIX,i);
			lex_exec_nowait(strtmp);
		}
		exit(0);
		break;
    case 53:
		umask(0113);
		if ((strcmp(DB_USER,"cperson")!=0)&&
		   (strcmp(DB_USER,"brian")!=0))
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Higher Access Level required to perform this operation");
		if (sscanf(entries[0].val,"%ld",&i)!=1)
		{
			sprintf(strtmp,"Incorrect Cluster Number [%s]\n",entries[0].val);
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,strtmp);
		}
  		Sometime=time(&Sometime);
		setbuf(stdout,NULL);
 		setbuf(stderr,NULL);
		sprintf(strtmp2,"ReAnalysing %s%ld.Please Wait", DB_CPREFIX,i);
		print_cleartop(1,0,1,strtmp2,"white","black");
		sprintf(strtmp,"%s/tmp/%ld.reblast",LEXDB_ROOT_DIR,Sometime);
		if ((fd=fopen(strtmp,"w"))==NULL)
		{
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,strtmp);
		}
    		fprintf(fd,"%ld\n",i);
		fclose(fd);
		printf("<PRE>");
		sprintf(strtmp,"%s/bin/lex_blast_subsets_preclean %s %s/tmp/%ld.reblast",
				LEXDB_ROOT_DIR, DB_NAME,LEXDB_ROOT_DIR,Sometime);
		lex_exec_wait(strtmp);
		sleep(1);
		printf("</PRE>");

		printf("Reblast Subsets Command Completed for %s%ld.<BR>You may now close this window.",DB_CPREFIX,i);
#if 0
		sprintf(strtmp,"%s/tmp/%ld.reblast",LEXDB_ROOT_DIR,Sometime);
		remove(strtmp);
#endif
		printf("</BODY></HTML>");
		fflush(stdout);
		exit(0);
		break;
    case 54:
		sprintf(strtmp,"%s PFAM DB Hits Search Page",DB_HTML_NAME);
		print_cleartop(1,0,1,strtmp,"white","black");
		make_sub_cell("white","white","#209090");
		printf("%s<FONT COLOR=black>Search %s HMM hits against PFAMDB</FONT></FONT>",TIMES_10,DB_HTML_NAME);
		close_cell();
		printf("<A HREF=%s?LIST_PFAM_ORDER_BY_NAME TARGET=%s_PFAMILIES>%s<FONT COLOR=white>List Hit Protein Families with Names Starting With:</FONT></FONT></A><BR>",DB_NAME,DB_NAME,TIMES_10);
		make_sub_cell("white","white","#209090");
		printf("<TABLE BORDER=0 WIDTH=100%%>");
		printf("<TR>");
		for (i=65; i<91; i++)
		{
		    printf("<TH BGCOLOR=white><A HREF=\"javascript://\" onClick=\"window.open('%s?LIST_PFAM_ORDER_BY_NAME=%c','%s_PFAMILIES', 'scrollbars=yes,resizable=yes,width=600,height=400,top=10,left=10');\">%s<FONT COLOR=blue>%c</FONT></FONT></A></TH>",DB_NAME,toupper(i),DB_NAME,TIMES_10,toupper(i));
#if 0
		    printf("<TH BGCOLOR=white><A HREF=%s?LIST_PFAM_ORDER_BY_NAME=%c TARGET=%s_PFAMILIES>%s<FONT COLOR=blue>%c</FONT></FONT></A></TH>",DB_NAME,toupper(i),DB_NAME,TIMES_10,toupper(i));
#endif
		    if (i==77) printf("</TR><TR>");
		}
		printf("</TR><TR>");
		for (i=1; i<10; i++)
		    printf("<TH BGCOLOR=white><A HREF=%s?LIST_PFAM_ORDER_BY_NAME=%d TARGET=%s_PFAMILIES>%s<FONT COLOR=blue>%d</FONT></FONT></A></TH>",DB_NAME,i,DB_NAME,TIMES_10,i);
		printf("<TH BGCOLOR=white>&nbsp;</TH>");
		printf("<TH BGCOLOR=white>&nbsp;</TH>");
		printf("<TH BGCOLOR=white>&nbsp;</TH>");
		printf("</TR></TABLE>");
		close_cell();
		exit(0);
		break;
    case 55:
		if (strcmp(entries[1].name,"FAMILY")!=0)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Argument");
		if (sscanf(entries[0].val,"%ld",&i)!=1)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid Sequence Number");
		if (strcpy(strtmp2,entries[1].val)==NULL)
			print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,"Invalid PFAM Family Name");
		sprintf(strtmp,"%s%ld vs [%s] Protein Family HMM",DB_PREFIX,i,strtmp2);
		print_cleartop(1,0,1,strtmp,"white","black");
		make_sub_cell("white","white","#209090");
		printf("%s<FONT COLOR=black>HMM Search Results: %s%ld vs. %s Protein Family</FONT></FONT>",TIMES_10,DB_PREFIX, i, strtmp2);
		close_cell();
		make_sub_cell("white","white","#209090");
		sprintf(strtmp3,"%s/databases/%s/analysis/pfamdb/%s.out",
			LEXDB_ROOT_DIR, DB_NAME, DB_NAME);
		already=0;
		if ((fd=fopen(strtmp3,"r"))!=NULL)
		{
		   sprintf(strtmp3,"/%s.hmmo",strtmp2);
		   printf("%s<FONT COLOR=black><PRE>",COURIER_10);
		   while (!feof(fd))
		   {
		     if (fgets(line,1024,fd)!=NULL)
		     {
		       if (!already)
		       {
		         if (strncmp(line,"Query=",6)==0)
		      	 {
			    if (strstr(line,strtmp3)!=NULL)
			    {
				printf("%s",line);
			        already=1;
			    }
			 }
		       }
		       else
		       {
		         if (strncmp(line,"HFRAME",6)==0)
			 {
			    printf("</PRE></FONT></FONT>");
			    close_cell();
			    exit(0);
			 }
			 else
				printf("%s",line);
		       }
		     }
		   }
		}
		close_cell();
		exit(0);
		break;
    case 56:
		sprintf(strtmp,"Accession Numbers to %s Cluster ID matching",DB_HTML_NAME);
		print_cleartop(1,0,1,strtmp,"white","black");
                dbproc=(DBPROCESS *)lex_get_dbproc(LEXDB_SYBASE_SERVER,DB_SYBASE_PROD_NAME);
		dbfcmd(dbproc,"select id,filename from ABIFile_Table where filename like '%%ACCESSION:%%%s%% %%' and id>0", entries[0].val);
		dbsqlexec(dbproc);
		while ((rt=dbresults(dbproc))!=NO_MORE_RESULTS)
		{
	  	    if (rt==SUCCEED)
		    {
			dbbind(dbproc,1,INTBIND,(DBINT)0,(BYTE *)&i);
			dbbind(dbproc,2,STRINGBIND,(DBINT)0,(BYTE DBFAR *)strtmp);
			while (dbnextrow(dbproc)!=NO_MORE_ROWS)
			{
			    if (!already)
			    {
				make_sub_cell("white","white","#209090");
				printf("<TABLE BORDER=0 WIDTH=100%%><TR>");
				printf("<TH BGCOLOR=#209090>%s<FONT COLOR=black>%s Cluster Number</FONT></FONT></TH>", TIMES_10, DB_CPREFIX);
				printf("<TH BGCOLOR=#209090>%s<FONT COLOR=black>Fasta Definition</FONT></FONT></TH>", TIMES_10, DB_CPREFIX);
				printf("</TR>");
				already=1;
			    }
			    printf("<TR>");
			    printf("<TH BGCOLOR=white>%s<FONT COLOR=black>%s%ld</FONT></FONT></TH>", TIMES_10, DB_CPREFIX,i);
			    printf("<TH BGCOLOR=#209090>%s<FONT COLOR=black>%s</FONT></FONT></TH>", TIMES_10, strtmp);
			    printf("</TR>");
				
			}
		    }
		}
		if (already)
		{
			printf("</TABLE>");
			close_cell();
		}
		else
				printf("%s<FONT COLOR=white>No Such Accession [%s] in %s</FONT></FONT></TH>", TIMES_10, entries[0].val,DB_HTML_NAME);
			
		break;
    default:
		sprintf(strtmp,"Invalid Query: %s",entries[0].name);
		print_error_page("BLACK","WHITE","MAGENTA",TIMES_12,strtmp);
		break;
  }
  
  if (ispost) free_multiform(mf);
  exit(0);
}

