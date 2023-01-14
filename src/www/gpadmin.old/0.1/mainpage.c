#include <stdio.h>
#include <stdlib.h>
#include "gpconfig.h"
#include "gpadmin.h"
#include "gphtml.h"


void do_main_page(int i,char *ob_user)
{
  printf("<CENTER><BR><A HREF=%s_admin?GP_PENDING=1>Check New Pedigrees</A><p>\n",DB_NAME);
  printf("<A HREF=%s_admin?GP_VALID=1>List Valid Pedigrees</A><p>\n",DB_NAME);
  printf("<A HREF=%s_admin?GP_QUESTIONS=1>GenPed Questionnaires</A><p>\n",DB_NAME);
  printf("<A HREF=%s_db>%s Public Database Web Interface</A><p>\n",DB_NAME,DB_HTML_NAME);
  printf("<A HREF=%s_admin?GP_USERS=1>Users Administration</A><p>\n",DB_NAME);
  printf("<A HREF=%s_admin?GP_HELP=0>GenPed Online Documentation</A><BR>\n",DB_NAME);
  printf("&nbsp;");
  printf("</CENTER>");
}
