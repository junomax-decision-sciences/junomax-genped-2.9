/* FORM2HTML Library Version 1.1 <cperson@lexgen.com> Last Changed: 09/16/1999 */
/* This library is used when one wants to have one text file per HTML form */

/* THERE ARE 80 TOKENS recognized in the conf file syntax */
#define MAX_TOKENS	80

/* Read the form.conf file and output the corresponding HTML FORM TOKENS AFTER
you have made the call to <FORM METHOD=POST ACTION=...> and before you close theoform (</FORM>) */

void form2html(char *conf_filename);

/* Supported TOKENS includes (see form0.conf example file) */
/*
*0*	"START_RADIO"		 Start RADIO INPUT *
*1*	"RADIO_TITLE="		 The single sentence above the radio button *
*2*	"RADIO="		 The NAME (label) for the RADIO button(s) *
*3*	"RADIO_VALUE="		 Default NON CHECKED VALUE *
*4*	"RADIO_SVALUE="	 Default CHECKED VALUE *
*9*	"END_RADIO"

*10*	"START_CHECKBOX"
*11*	"CHECKBOX_TITLE="	 A single sentence above the checkbox *
*12*	"CHECKBOX="		 The NAME for the CHECKBOX *
*13*	"CHECKBOX_VALUE="	 Default UNCHECKED VALUE *
*14*	"CHECKBOX_SVALUE="	 Default CHECKED VALUE *
*19*	"END_CHECKBOX"	

*20*	"START_MENU"
*21*	"MENU_TITLE="		
*22*	"MENU="		 The NAME for this MENU *
*23*	"MENU_VALUE="		 Default NON SELECTED VALUE(S) *
*24*	"MENU_SVALUE="		 Default SELECTED VALUE *
*25*	"MENU_SIZE="		 The SIZE= for this menu *
*29*	"END_MENU"

*30*	"START_MULTIPLE"
*31*	"MULTIPLE_TITLE="
*32*	"MULTIPLE="		 The NAME for this MULTIPLE *
*33*	"MULTIPLE_VALUE="	 Default NON SELECTED VALUE(S) *
*34*	"MULTIPLE_SVALUE="	 Default SELECTED VALUE(S) *
*35*	"MULTIPLE_SIZE="	 The SIZE= for this multiple *
*39*	"END_MULTIPLE"

*40*	"START_TEXTAREA"
*41*	"TEXTAREA_TITLE="
*42*	"TEXTAREA="		 The NAME for this TEXTAREA *
*43*	"TEXTAREA_VALUE="	 Default VALUE FOR TEXTAREA *
*44*	"TEXTAREA_COLS="	 The COLS= for this TEXTAREA *
*45*	"TEXTAREA_ROWS="	 The ROWS= for this TEXTAREA *
*49*	"END_TEXTAREA"


*50*	"START_TEXT"
*51*	"TEXT_TITLE="
*52*	"TEXT="			 The NAME for this TEXT *
*53*	"TEXT_VALUE="		 Default VALUE for this TEXT *
*54*	"TEXT_SIZE="		 Default SIZE= for this TEXT *
*59*	"END_TEXT"

*70*	"START_HTML"		 HTML (convenient for formatting the page
*79*	"END_HTML"		 around other TOKENS
*/
