#define GP_HTML_VERSION    "V2.01"

/* Some Font tags */


#define TIMES_6		"<FONT FACE=\"Times\" POINT-SIZE=\"6\">"
#define TIMES_8		"<FONT FACE=\"Times\" POINT-SIZE=\"8\">"
#define TIMES_10	"<FONT FACE=\"Times\" POINT-SIZE=\"10\">"
#define TIMES_12	"<FONT FACE=\"Times\" POINT-SIZE=\"12\">"
#define TIMES_14	"<FONT FACE=\"Times\" POINT-SIZE=\"14\">"
#define TIMES_18	"<FONT FACE=\"Times\" POINT-SIZE=\"18\">"
#define TIMES_24	"<FONT FACE=\"Times\" POINT-SIZE=\"24\">"

#define HELV_6		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"6\">"
#define HELV_8		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"8\">"
#define HELV_10		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"10\">"
#define HELV_12		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"12\">"
#define HELV_14		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"14\">"
#define HELV_18		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"18\">"
#define HELV_24		"<FONT FACE=\"Helvetica\" POINT-SIZE=\"24\">"

#define ARIAL_6		"<FONT FACE=\"Arial\" POINT-SIZE=\"6\">"
#define ARIAL_8		"<FONT FACE=\"Arial\" POINT-SIZE=\"8\">"
#define ARIAL_10	"<FONT FACE=\"Arial\" POINT-SIZE=\"10\">"
#define ARIAL_12	"<FONT FACE=\"Arial\" POINT-SIZE=\"12\">"
#define ARIAL_14	"<FONT FACE=\"Arial\" POINT-SIZE=\"14\">"
#define ARIAL_18	"<FONT FACE=\"Arial\" POINT-SIZE=\"18\">"
#define ARIAL_24	"<FONT FACE=\"Arial\" POINT-SIZE=\"24\">"

#define COURIER_4          "<FONT FACE=\"Courier\" POINT-SIZE=\"4\">"
#define COURIER_6          "<FONT FACE=\"Courier\" POINT-SIZE=\"6\">"
#define COURIER_8          "<FONT FACE=\"Courier\" POINT-SIZE=\"8\">"
#define COURIER_10         "<FONT FACE=\"Courier\" POINT-SIZE=\"10\">"
#define COURIER_12         "<FONT FACE=\"Courier\" POINT-SIZE=\"12\">"
#define COURIER_14         "<FONT FACE=\"Courier\" POINT-SIZE=\"14\">"
#define COURIER_18         "<FONT FACE=\"Courier\" POINT-SIZE=\"18\">"
#define COURIER_24         "<FONT FACE=\"Courier\" POINT-SIZE=\"24\">"

#define DEF_FONT	TIMES_12


/* Print the common top and print the PRIMARY cell */
void print_top(char menu,char cache, char *title, char *fg, char *bg);

/* An empty top */
void print_cleartop(char contenttype, char menu,char cache, char *title, char *fg, char *bg);

/* Make another cell */
void make_sub_cell(char *fg, char *bg, char *bordercol);

/* Closes the latest opened cell */ 
void close_cell();

