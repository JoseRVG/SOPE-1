#ifndef SOPE_P1_SRC_RFENT_H_
#define SOPE_P1_SRC_RFENT_H_

#include <sys/stat.h> //struct stat

#include "sizelim.h"

//rfent stands for "regular file entry"

struct rfent
{
	char d_name[MAX_D_NAME];
	char d_full_name[MAX_D_FULL_NAME];
	struct stat stat_buf;
};

void copy(struct rfent *dest, struct rfent *src);
int equals(struct rfent r1, struct rfent r2);
int shallow_equals(struct rfent r1, struct rfent r2);

#endif /* SOPE_P1_SRC_RFENT_H_ */
