#include <string.h> //strcpy

#include "rfent.h"

#include "compfiles.h" //compareFiles

void copy(struct rfent *dest, struct rfent *src)
{
	strcpy(dest->d_name,src->d_name);
	strcpy(dest->d_full_name,src->d_full_name);
	dest->stat_buf = src->stat_buf;
}

int equals(struct rfent r1, struct rfent r2)
{
	if(shallow_equals(r1,r2)) //to try to avoid calling compareFiles...
	{
		//Compare by contents
		return compareFiles(r1.d_full_name,r2.d_full_name);
	}
	return 0; //false
}

int shallow_equals(struct rfent r1, struct rfent r2)
{
	//Compare by name
	if(strcmp(r1.d_name,r2.d_name) == 0)
	{
		//Compare by mode_t (permissions...)
		if(r1.stat_buf.st_mode == r2.stat_buf.st_mode)
		{
			//Compare by size (to try to avoid calling compareFiles...)
			if(r1.stat_buf.st_size == r2.stat_buf.st_size)
			{
				return 1; //true
			}
		}
	}
	return 0; //false
}
