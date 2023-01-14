#include <sys/types.h>

#define MAX_FILENAME	255
#define MAX_NAME	8

#define MIN_INDIV_WIDTH         20
#define MIN_INDIV_HEIGHT        20

typedef struct INDIV_struct {
	int id;
	char sex;	/* 0: female	1: male */
	int father;	/* Father id */
	int mother;	/* Mother id */
	int spouse;	/* Spouse id */
	int kid;	/* First kid id */
	int sib;	/* Next sibling id */
	char proband;	/* 0 or 1 pedigree contributor */
	int pheno;	/* To be defined... */
	int generation;
	double ratio;
    	double x;
	double y;
	double w;
	double h;
	int xpic;
	int ypic;
} INDIV;

typedef struct COUPLE_struct {
	int id;
	int father_id;
	int mother_id;
	int nb_sdaughters;
	int nb_cdaughters;
	int nb_csons;
	int nb_ssons;
} COUPLE;

typedef struct box_struct {
  int indiv_id; /* If -1, empty box , i.e. under kids in older generation */
  double ratio;
  int xpos;
  int ypos;
  int w;
  int y;
} INDIV_BOX;

typedef struct GENERATION_struct {
	int id;
	int nbcouples;
	int *couples;
	int nbindivs;
	int *indivs;
	int nbboxes;
	INDIV_BOX **boxes;
} GENERATION;

/* The pedigree structure is the only one who carries the pointers */
typedef struct PEDIGREE_struct {
	long id;		/* Can be used to store time_t as ticket */
	char filename[255];
	int nbindivs;
	INDIV **indivs;
	int nbcouples;
	COUPLE **couples;
	int nbgenerations;
	GENERATION **generations;
} PEDIGREE; 

PEDIGREE *load_pedigree(char *filename,char onlyload);

int gif2pedigree(PEDIGREE *p, char *filename, int draw_pheno);
