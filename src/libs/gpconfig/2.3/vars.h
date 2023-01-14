#include <sys/types.h>
#include <time.h>
#include "gp_colors.h"

/* OPTIONAL */
/* Provided either by getenv or throu SetVar in the httpd server srm.conf */

#define DEFAULT_GP_ROOT_DIR	"/home/genped"

char GP_ROOT_DIR[255]={ DEFAULT_GP_ROOT_DIR };

char DB_USER[255]={""};
/* General Globals for lexdb program */
uid_t GP_UID = 0;
gid_t GP_GID = 0;

char GP_LOCAL_DOMAIN[255]={""};

char GP_SUPERUSER_AUTH_FILE[255]={""};

/* Specific database global variable */
char DB_NAME[255]={""};
char DB_HTML_NAME[255]={""};
char DB_EXTENDED_NAME[2048]={""};

/* Global variables for database a specific DB_NAME */
/* Loaded from DB_NAME.conf */
 
char DB_OWNER[30]={""};

char DB_PREFIX[255]={""};

/* Global variables for the web pages colors */
char bgcol[GP_MAX_COLORS][8]=GP_COLORS_ARRAY;

/* This variable is the GIF filename for a database specific logo */
char DB_LOGOGIF_FILE[255]={""};
int DB_LOGOGIF_WIDTH=0;
int DB_LOGOGIF_HEIGHT=0;

char DB_ICONGIF_FILE[255]={""};
int DB_ICONGIF_WIDTH=0;
int DB_ICONGIF_HEIGHT=0;


/* All questions expect Yes/No answer !!! -> Radio Button */
typedef struct gp_question_struct {
    int id;
    char type;	/* 0: checkbox  1: radio  */
    int name;	/* passed as NAME= */
    char label[255];
    char question[255];
} GP_QUESTION;

/* Each section starts with a <HR> */
typedef struct gp_section_struct {	/* Starts with SECTION 1 */
    int id;				/* Serial number */
    char title[255];			/* Description or question */
    char comments[1024];		/* Comments file */
    int nbquestions;
    GP_QUESTION **questions;
} GP_SECTION;				/* End at next SECTION or eof */

int DB_NBSECTIONS=0;			/* From GenPed HTML Form config file */
GP_SECTION **DB_SECTIONS=NULL;

char DB_DISORDER_NAME[255]={""};

char GP_HTDOCS_DIR[255]={""};

char DB_FONT_COLOR[255]={""};

char GP_MAX_GENERATIONS=0;		/* MAXIMUM IS 5, if 0 then ignored */	
