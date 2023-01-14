#include <sys/types.h>
#include <time.h>
#include "gp_colors.h"

/* All questions expect Yes/No answer !!! -> Radio Button */
typedef struct gp_question_struct {
    int id;
    char type;  /* 0: checkbox  1: radio  */
    int name;   /* passed as NAME= */
    char label[255];
    char question[255];
} GP_QUESTION;

/* Each section starts with a <HR> */
typedef struct gp_section_struct {      /* Starts with SECTION 1 */
    int id;                             /* Serial number */
    char title[255];                    /* Description or question */
    char comments[1024];                /* Comments file */
    int nbquestions;
    GP_QUESTION **questions;
} GP_SECTION;                           /* End at next SECTION or eof */

extern char GP_ROOT_DIR[255];

extern char DB_USER[255];

/* General Globals for lexdb program */
extern uid_t GP_UID;
extern gid_t GP_GID;

extern char GP_LOCAL_DOMAIN[255];

extern char GP_SUPERUSER_AUTH_FILE[255];

/* Specific database global variable */
extern char DB_NAME[255];
extern char DB_HTML_NAME[255];
extern char DB_EXTENDED_NAME[2048];

/* Global variables for database a specific DB_NAME */
/* Loaded from DB_NAME.conf */
 
extern char DB_OWNER[30];

/* Global variables for the web pages colors */
extern char bgcol[GP_MAX_COLORS][8];

/* If this is triggered, then "annotators" can comments and classify OSTs */
/* Specific to a database */
extern char DB_HAS_MEMO;

/* This variable from db.conf overwrite GP_CGIBIN_DIR when found */
extern char DB_CGIBIN_DIR[255];

/* This variable is the GIF filename for a database specific logo */
extern char DB_LOGOGIF_FILE[255];
extern int DB_LOGOGIF_WIDTH;
extern int DB_LOGOGIF_HEIGHT;

/* This variable is the GIF filename for a database specific logo */
extern char DB_LOGOGIF_FILE[255];
extern int DB_LOGOGIF_WIDTH;
extern int DB_LOGOGIF_HEIGHT;

extern char DB_ICONGIF_FILE[255];
extern int DB_ICONGIF_WIDTH;
extern int DB_ICONGIF_HEIGHT;

extern int DB_NBSECTIONS;		/* From GenPed HTML Form config file */
extern GP_SECTION **DB_SECTIONS;

extern char DB_DISORDER_NAME[255];
extern char GP_HTDOCS_DIR[255];
extern char DB_FONT_COLOR[255];

extern char GP_MAX_GENERATIONS;
