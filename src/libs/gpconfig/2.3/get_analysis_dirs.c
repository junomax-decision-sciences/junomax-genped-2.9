#include <stdio.h>
#include <unistd.h>

#include "lexconfig.h"
 

/* This gives the analysis_dir for an OST */
/* Filename has to be already malloc'd or static */

void get_ost_dir(char *analysis_dir, long ostid)
{
long thous=0;
long hunds=0;
 
          thous=(long)(ostid/1000);
          hunds=(long)(ostid%1000)/100;
          if (thous==0)
          {
           if (hunds==0)
             sprintf(analysis_dir,"%s/0/0/%s%ld",DB_SEQUENCES_DIR, DB_PREFIX,
			ostid);
           else
             sprintf(analysis_dir,"%s/0/%ld00/%s%ld",DB_SEQUENCES_DIR,
			hunds,DB_PREFIX,ostid);
          }
          else
          {
           if (hunds==0)
             sprintf(analysis_dir,"%s/%ld000/0/%s%ld",DB_SEQUENCES_DIR, thous,
			DB_PREFIX,ostid);
            else
             sprintf(analysis_dir,"%s/%ld000/%ld00/%s%ld",
		DB_SEQUENCES_DIR, thous, hunds,DB_PREFIX,ostid);
          }
}

void get_oc_dir(char *analysis_dir, long ocid)
{
long thous=0;
long hunds=0;

          thous=(long)(ocid/1000);
          hunds=(long)(ocid%1000)/100;
          if (thous==0)
          {
           if (hunds==0)
             sprintf(analysis_dir,"%s/0/0/%s%ld",DB_CLUSTERS_DIR, DB_CPREFIX,
                        ocid);
           else
             sprintf(analysis_dir,"%s/0/%ld00/%s%ld",DB_CLUSTERS_DIR,
                        hunds,DB_CPREFIX,ocid);
          }
          else
          {
           if (hunds==0)
             sprintf(analysis_dir,"%s/%ld000/0/%s%ld",DB_CLUSTERS_DIR,
                        thous,DB_CPREFIX,ocid);
            else
             sprintf(analysis_dir,"%s/%ld000/%ld00/%s%ld",
                DB_CLUSTERS_DIR, thous,hunds,DB_CPREFIX,ocid);
          }
}

