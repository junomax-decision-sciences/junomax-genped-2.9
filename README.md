# junomax-genped-2.9
Author: Christophe Person (christophejperson@gmail.com)

	Wrote this around 1996 for the Epilepsy Foundation of America in collaboration with Professor Jeff Noebels.
 
	Generate simple GIF images (genealogic trees) from a .ped textfile.
 
Format of .ped files: one line per individual
 
 id	sex	mother_id father_id	spouse_id first_kid_id next_sibling_id proband phenotype
 
 Each map to a single Individual
    int id;     /* Unique Id for each individual/row in the file */
	char sex;	/* 0: female	1: male */
	int father;	/* Father id */
	int mother;	/* Mother id */
	int spouse;	/* Spouse id */
	int kid;	/* First kid id */
	int sib;	/* Next sibling id */
	char proband;	/* 0 or 1 pedigree contributor */
	int pheno;	/* Phenotype_id */
	
 The junomax-genped-2.9/src/libs/libped/2.9/ directory contains the libped C library and standalone example C program, and can be copied individually.
 (TODO: make a github repository for genped-libped C library.
 
