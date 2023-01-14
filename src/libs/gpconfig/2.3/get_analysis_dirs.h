#include <stdio.h>
#include <unistd.h>

#include "lexconfig.h"
 

/* This gives the analysis_dir for an OST */
/* Filename has to be already malloc'd or static */

void get_ost_dir(char *analysis_dir, long ostid);

void get_oc_dir(char *analysis_dir, long ocid);

