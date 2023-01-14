#include <stdio.h>
#include <stdlib.h>
#include "gpconfig.h"
#include "gpadmin.h"
#include "gphtml.h"


void do_main_page()
{
  printf("&nbsp;");
  printf("<CENTER><BR>%s<FONT COLOR=black>GenPed Administrator Interface</FONT></FONT><BR>",TIMES_18);
}

void do_pedig_page()
{
  printf("<CENTER><BR>%s<FONT COLOR=black>GenPed Administrator Interface</FONT></FONT><BR>",TIMES_18);
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
