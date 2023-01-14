#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpconfig.h"

main(int argc, char **argv)
{
  if (argc!=2)
  {
	printf("Usage: example DB_NAME\n");
	exit(1);
  } 
  gp_get_config(argv[1]);


  printf("GP_ROOT_DIR=[%s]\n",GP_ROOT_DIR);
  printf("GP_UID=[%d]\n",GP_UID);
  printf("GP_GID=[%d]\n",GP_GID);
  printf("GP_LOCAL_DOMAIN=[%s]\n",GP_LOCAL_DOMAIN);

  printf("\nDB_NAME=[%s]\n",DB_NAME);
  printf("DB_OWNER=[%s]\n",DB_OWNER);
  printf("DB_USER=[%s]\n",DB_USER);
  printf("DB_HTML_NAME=[%s]\n",DB_HTML_NAME);
  printf("DB_EXTENDED_NAME=[%s]\n",DB_EXTENDED_NAME);
  printf("DB_LOGOGIF_FILE=[%s]\n",DB_LOGOGIF_FILE);
  printf("DB_LOGOGIF_FILE=[%s]\n",DB_LOGOGIF_FILE);
  printf("DB_LOGOGIF_WIDTH=[%d]\n",DB_LOGOGIF_WIDTH);
  printf("DB_ICONGIF_HEIGHT=[%d]\n",DB_ICONGIF_HEIGHT);
  printf("DB_ICONGIF_WIDTH=[%d]\n",DB_ICONGIF_WIDTH);
  printf("DB_ICONGIF_HEIGHT=[%d]\n",DB_ICONGIF_HEIGHT);
  printf("GP_HTDOCS_DIR=[%s]\n",GP_HTDOCS_DIR);

}
