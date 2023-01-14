#include <stdio.h>
#include <stdlib.h>
#include "gpconfig.h"
#include "gpquery.h"
#include "gphtml.h"


void do_main_page()
{
  printf("<CENTER><BR>%s<FONT COLOR=black>What is this project about  ?</FONT><p></FONT>\n",TIMES_18);
  printf("<TABLE BORDER=0 WIDTH=90%%><TR><TD BGCOLOR=white WIDTH=100%%><B>%sThis is an international search on the World Wide Web for families with %s in order to:</FONT></b></TD></TR></TABLE><BR>\n",TIMES_14,DB_DISORDER_NAME);
  printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=20%%></TH><TD BGCOLOR=white>");
  printf("%sEducate persons affected by %s about new genetic research in this disorder.<p>\n",TIMES_12,DB_DISORDER_NAME);
  printf("Enlist families with hereditary %s for future participation in clinical genetic research projects by filling out an anonymous questionnaire.<p>\n",DB_DISORDER_NAME);
  printf("Match those interested in volunteering their pedigree with researchers in genetics in order to identify the %s gene in your family<p>\n",DB_HTML_NAME);
  printf("</TD></TR></TABLE><p>");
  printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=10%%></TH><TD BGCOLOR=white WIDTH=80%%>");
  printf("In the next pages, you will be guided to construct your <i>pedigree</i>, a sort of genealogic tree used by researchers to establish the individual <i>links</i> to %s among family members. Then, the completion of a brief form will allow scientists to determine whether the pattern of %s found in your family could be valuable for further study.<br>\n",DB_DISORDER_NAME,DB_DISORDER_NAME);
  printf("</TD><TH BGCOLOR=white WIDTH=10%%></TH></TR></TABLE><BR>");
  printf("<CENTER><BR>%s<A HREF=%s?GP_WHATPED=1><FONT COLOR=%s>What is a pedigree ?</FONT></A><p></FONT>\n",TIMES_14,DB_NAME,bgcol[2]);
  printf("%s<A HREF=%s?GP_BUILD=1><FONT COLOR=%s>Build your own pedigree</FONT></A><p></FONT>\n",TIMES_14,DB_NAME,bgcol[2]);
  printf("&nbsp;");
  printf("</CENTER>");
}

void do_pedig_page()
{
  printf("<CENTER><BR>%s<FONT COLOR=black>What is a pedigree ?</FONT></FONT><BR>",TIMES_18);

  printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=white WIDTH=384 HEIGHT=200><IMG SRC=/genped/gifs/ped-example.gif WIDTH=384 HEIGHT=200></TH><TH>%s\n",TIMES_12);
  printf("A <I>pedigree</I> is used by researchers to show how a gene <i>travels</i> between individuals in successive generations of a family.<p>\n");
  printf("Female individuals are shown as <FONT COLOR=red>red circles</FONT> and male individuals as <FONT COLOR=blue>blue squares</FONT>.</FONT>");
 printf("</TH></TR></TABLE><BR>");
  printf("<CENTER><TABLE BORDER=0 WIDTH=80%%><TR><TH BGCOLOR=white WIDTH=100%%>%sNow follow this link to <A HREF=%s?GP_BUILD=1><FONT COLOR=%s>build your own pedigree</FONT></A>.<br>Once finished, you will be able to save it as a picture or print it.<p></FONT></TH></TR></TABLE>\n",TIMES_14,DB_NAME,bgcol[2]);

}

void do_first_page()
{
time_t Sometime;
  printf("<FORM METHOD=GET ACTION=%s>\n",DB_NAME);
  printf("<INPUT TYPE=HIDDEN NAME=BUILD_PED VALUE=1></INPUT>\n");
  printf("<CENTER><BR>%s<FONT COLOR=black><u>Please answer to the following questions below:</U></FONT></FONT><P>",TIMES_14);
  printf("<TABLE BORDER=0 WIDTH=70%%><TR><TD BGCOLOR=white WIDTH=60%%><B>%sAre you male or female ?</FONT></B></TD><TH BGCOLOR=white WIDTH=30%%><SELECT TYPE=MENU NAME=SEX><OPTION VALUE=1 SELECTED>Male</OPTION><OPTION VALUE=0>Female</OPTION></SELECT></TH></TR></TABLE><br>",TIMES_12);
  printf("<TABLE BORDER=0 WIDTH=70%%><TR><TD BGCOLOR=white WIDTH=60%%><B>%sOn which side of your family does %s run ?</FONT></B></TD><TH BGCOLOR=white WIDTH=30%%><SELECT TYPE=MENU NAME=SIDE><OPTION VALUE=1 SELECTED>Father's Side</OPTION><OPTION VALUE=0>Mother's Side</OPTION></SELECT></TH></TR></TABLE><br>",TIMES_12,DB_DISORDER_NAME);
  printf("<TABLE BORDER=0 WIDTH=70%%><TR><TD BGCOLOR=white WIDTH=60%%><B>%sDo you have any children, nieces or nephews ?</FONT></B></TD><TH BGCOLOR=white WIDTH=30%%><SELECT TYPE=MENU NAME=KIDS><OPTION VALUE=1 SELECTED>Yes</OPTION><OPTION VALUE=0>No</OPTION></SELECT></TH></TR></TABLE><br>",TIMES_12);
  printf("<TABLE BORDER=0 WIDTH=60%%><TR><TH BGCOLOR=white WIDTH=100%%>%s<INPUT TYPE=SUBMIT NAME=SUBMIT VALUE='Submit Answers'></INPUT></FONT></TH></TR></TABLE><br>",TIMES_10);
}
