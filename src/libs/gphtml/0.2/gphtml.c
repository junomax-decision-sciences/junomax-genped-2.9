#include <stdio.h>
#include "gp_colors.h"
#include "gpconfig.h"
#include "gphtml.h"

#define SPAN_BGCOLOR	"#90E0E0"
#define INFOSPAN_BGCOLOR "#000000"

void print_top(char menu,char cache, char *title, char *fg, char *bg)
{
  printf("Content-Type: text/html\r\n\r\n");
  printf("<HTML><HEAD><TITLE>%s</TITLE>\n",title);
  if (!cache) printf("<META HTTP-EQUIV='pragma' CONTENT='no-cache'>\n");
  printf("</HEAD>\n");
  printf("<BODY BGCOLOR=%s LINK=blue VLINK=blue ALINK=red TEXT=%s>\n",bg,fg);
  printf("<TABLE BORDER=0><TR><TH ALIGN=LEFT BGCOLOR=WHITE><IMG SRC=%s/gifs/genped-logo.gif ALT='GenPed Logo'></TH><TH BGCOLOR=WHITE ALIGN=RIGHT><IMG SRC=%s/gifs/ob-logo.gif ALT=''></TH></TR></TABLE>\n", GP_HTDOCS_DIR,GP_HTDOCS_DIR,GP_HTDOCS_DIR);
  make_sub_cell(fg, bg,DB_FONT_COLOR);
  printf("<TABLE BORDER=0 WIDTH=100%%><TR><TH BGCOLOR=%s ALIGN=left>%s<FONT COLOR=%s><CENTER>%s</FONT></FONT></TH>\n", bg,TIMES_14,fg,title);
  printf("</TR></TABLE>\n",bg);
  close_cell();
}

void print_cleartop(char contenttype,char menu,char cache, char *title, char *fg, char *bg)
{
  if (contenttype)
	printf("Content-Type: text/html\r\n");
  if (!cache)
	printf("Pragma: no-cache\r\n");
  printf("\r\n");
  printf("<HTML><HEAD><TITLE>%s</TITLE>\n",title);
  if (!cache) printf("<META HTTP-EQUIV='pragma' CONTENT='no-cache'>\n");
  printf("</HEAD>\n");
  printf("<BODY BGCOLOR=%s LINK=%s VLINK=%s ALINK=red TEXT=%s>\n",bg,fg,fg,fg);
  printf("<NOSCRIPT>This site uses Javascript functionality. Please enable Javascript in your browser Preferences or contact your system administrator for further help</NOSCRIPT>");
}
/* ONLY FOR show_cluster_list */
void print_cleartop2(char contenttype,char menu,char cache, char *title, char *fg
, char *bg)
{
  if (contenttype) printf("Content-Type: text/html\r\n\r\n");
  printf("<HTML><HEAD><TITLE>%s</TITLE>\n",title);
  if (!cache) printf("<META HTTP-EQUIV='pragma' CONTENT='no-cache'>\n");
  printf("<SCRIPT LANGUAGE=\"Javascript\">\nfunction show(object,e) {\nif (e != '') {\nif (document.all) {\n x = e.clientX;\ny = e.clientY;\n}\nif (document.layers) { x = e.pageX; y = e.pageY; } }\n");

  printf("if (document.layers && document.layers[object] != null) {\n document.layers[object].left = x; document.layers[object].top = y; } else if (document.all) { document.all[object].style.posLeft = x; document.all[object].style.posTop = y; }\n");
  printf("if (document.layers && document.layers[object] != null) document.layers[object].visibility = 'visible'; else if (document.all) document.all[object].style.visibility = 'visible'; }\n");
  printf(" function hide(object) { if (document.layers && document.layers[object] != null) document.layers[object].visibility = 'hidden'; else if (document.all) document.all[object].style.visibility = 'hidden'; }\n");
  printf("</SCRIPT>\n");

  printf("</HEAD>\n");
  printf("<BODY BGCOLOR=%s LINK=%s VLINK=%s ALINK=red TEXT=%s >\n",bg,fg,fg,fg);
  printf("<STYLE TYPE=\"text/css\">\nall.myStyle {\nfont-family: monospace;\nfont-size: 8pt;\nborder-style: none;\nposition: absolute;\nvisibility: hidden;\n color:black;\n background-color:%s;\n border-width:6pt;\npadding:6pt;\n }\n </STYLE>\n",SPAN_BGCOLOR);
#if 0
  printf("<STYLE TYPE=\"text/css\">\nall.myStyle {\nposition: absolute;\n visibility: hidden;\n color:black;\n background-color:#50C0C0;\n border-color:none; border-width:2pt;\n padding:6pt;\n }\n </STYLE>\n");
#endif
}
/* ONLY FOR show_cluster_list panel */
void print_cleartop3(char contenttype,char menu,char cache, char *title, char *fg
, char *bg)
{
  if (contenttype) printf("Content-Type: text/html\r\n\r\n");
  printf("<HTML><HEAD><TITLE>%s</TITLE>\n",title);
  if (!cache) printf("<META HTTP-EQUIV='pragma' CONTENT='no-cache'>\n");
  printf("<SCRIPT LANGUAGE=\"Javascript\">\nfunction show(object,e) {\nif (e != '') {\nif (document.all) {\n x = e.clientX;\ny = e.clientY;\n}\nif (document.layers) { x = e.pageX; y = e.pageY; } }\n");

  printf("if (document.layers && document.layers[object] != null) {\n document.layers[object].left = x; document.layers[object].top = y; } else if (document.all) { document.all[object].style.posLeft = x; document.all[object].style.posTop = y; }\n");
  printf("if (document.layers && document.layers[object] != null) document.layers[object].visibility = 'visible'; else if (document.all) document.all[object].style.visibility = 'visible'; }\n");
  printf(" function hide(object) { if (document.layers && document.layers[object] != null) document.layers[object].visibility = 'hidden'; else if (document.all) document.all[object].style.visibility = 'hidden'; }\n");
  printf("</SCRIPT>\n");

  printf("</HEAD>\n");
  printf("<BODY BGCOLOR=%s LINK=%s VLINK=%s ALINK=red TEXT=%s >\n",bg,fg,fg,fg);
  printf("<STYLE TYPE=\"text/css\">\nall.myStyle {\nfont-family: monospace;\nfont-size: 8pt;\nborder-style: none;\nposition: absolute;\nvisibility: hidden;\n color:black;\n background-color:%s;\n border-width:6pt;\npadding:6pt;\n }\n </STYLE>\n",INFOSPAN_BGCOLOR);
}


/* print_toperr for query program error messages */
void print_toperr(char menu,char cache, char *title, char *fg, char *bg)
{
  printf("Content-Type: text/html\r\n\r\n");
  printf("<HTML><HEAD><TITLE>%s</TITLE>\n",title);
  if (!cache) printf("<META HTTP-EQUIV='pragma' CONTENT='no-cache'>\n");
  printf("</HEAD>\n");
  printf("<BODY BGCOLOR=white LINK=blue VLINK=darkmagenta ALINK=red TEXT=black>\n");
}

void make_sub_cell(char *fg, char *bg, char *bordercol)
{
  printf("<TABLE WIDTH=100%% BORDER=0><TR><TH BGCOLOR=%s><TABLE BORDER=2 WIDTH=100%%><TR><TD BGCOLOR=%s>",bordercol,bg);
  printf("%s<FONT COLOR=%s>",TIMES_12,fg); /* Putting default font tag */
}

void make_sub_cell2(char *fg, char *bg, char *bordercol)
{
  printf("<TABLE BORDER=0><TR><TH BGCOLOR=%s><TABLE BORDER=2 WIDTH=100%%><TR><TD BGCOLOR=%s>",bordercol,bg);
  printf("%s<FONT COLOR=%s>",TIMES_12,fg); /* Putting default font tag */
}

void close_cell()
{
  printf("</FONT></TD></TR></TABLE></TH></TR></TABLE>");
}
