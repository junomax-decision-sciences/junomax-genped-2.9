#include <time.h>

#define YES_STR         "Yes"
#define NO_STR          "No"

#define POST_STR	"POST"
#define GET_STR		"GET"

#define MAX_MODES       7

#define QMODE_YESNO             0
#define QMODE_RADIO             1
#define QMODE_VALUE             2
#define QMODE_MENU              3
#define QMODE_LIST              4
#define QMODE_TEXTAREA          5
#define QMODE_MULTI             6



typedef struct QUESTION_struct {
  int id;
  char *question;
  int mode;	    /* Depending on mode, there can be several tokens */
  int nbtokens;
  char **tokens;    /* Each token constitute a choice or value description */
  char *value; /* the answer is stored in value as a string of variable size */
}QUESTION;

typedef struct HTMLFORM_struct {
  int id;
  char *action;
  int method;
  int nbquestions;
  QUESTION **questions;
  time_t last_change;
}HTMLFORM;

/* Reads a questionary file */
HTMLFORM *load_htmlform(char *filename);

int show_questions();
