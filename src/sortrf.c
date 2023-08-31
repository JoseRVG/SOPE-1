#include <unistd.h> //read
#include <fcntl.h> //open macros
#include <stdio.h> //perror
#include <string.h> //strlen

#include "sortrf.h"

static int lessThan(struct rfent r1, struct rfent r2)
{
	//Compare by file name
	int name_comp = strcmp(r1.d_name,r2.d_name);
	if(name_comp < 0)
	{
		return 1; //true
	}
	else if(name_comp == 0) //same name
	{
		//Compare by mode_t (permissions...)
		if(r1.stat_buf.st_mode < r2.stat_buf.st_mode)
		{
			return 1; //true
		}
		else if(r1.stat_buf.st_mode == r2.stat_buf.st_mode)
		{
			//Compare by size
			if(r1.stat_buf.st_size < r2.stat_buf.st_size)
			{
				return 1; //true
			}
			else if(r1.stat_buf.st_size == r2.stat_buf.st_size)
			{
				//Compare by modification dates
				if((int)(r1.stat_buf.st_mtim.tv_sec) < (int)(r2.stat_buf.st_mtim.tv_sec))
				{
					return 1; //true
				}
			}
		}
	}
	return 0; //false
}

static void swap(struct rfent *r1, struct rfent *r2)
{
	struct rfent temp; //temporary copy of r1
	copy(&temp,r1);
	copy(r1,r2);
	copy(r2,&temp);
}

static void bubbleRFSort(struct rfent entries[], unsigned int num_entries)
{
	if(num_entries == 0) return;
	unsigned int j;
	for(j=num_entries-1; j>0; j--)
	{
		int troca = 0; //false
		unsigned int i;
		for(i = 0; i<j; i++)
		{
			if(lessThan(entries[i+1], entries[i]))
			{
				swap(&entries[i+1],&entries[i]);
				troca = 1;
			}
		}
		if (!troca) break;
	}
}

int sortRfFile(char* filename, struct rfent entries[], unsigned int *num_entries, unsigned int max_entries)
{
	//Step 1: Read the entries
	int fd;

	//Step 1a: Open the file for reading
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		write(STDERR_FILENO, filename, strlen(filename));
		perror(" - open");
		return 2;
	}

	//Variables to process the regular file entry's contents
	struct rfent rf_buf;

	//Step 1b: Read the file
	int num_read;

	while ((num_read = read(fd,&rf_buf,sizeof(rf_buf))) == sizeof(rf_buf))
	{
		if(*num_entries >= max_entries)
		{
			char* too_many_entries = "Too many regular file entries in the file!\n";
			write(STDERR_FILENO,too_many_entries,strlen(too_many_entries));
			close(fd);
			return 2;
		}
		copy(&entries[*num_entries],&rf_buf);
		(*num_entries)++;
	}
	if(num_read == -1) //read failed
	{
		perror("read");
		close(fd);
		return 2;
	}

	//Step 1c: Close the file
	if(close(fd) == -1)
	{
		perror("close");
		return 2;
	}

	//Step 2: Sort the entries
	bubbleRFSort(entries,*num_entries);

	//Step 3: Write back the entries

	//Step 3a: Open the file for writing
	fd = open(filename, O_WRONLY|O_TRUNC);
	if (fd == -1)
	{
		perror("open");
		return 2;
	}

	//Step 3b: Write to the file
	unsigned int i;
	for(i = 0; i < *num_entries; i++)
	{
		if(write(fd,&entries[i],sizeof(entries[i])) != sizeof(entries[i]))
		{
			perror("write");
			close(fd);
			return 2;
		}
	}

	//Step 3c: Close the file
	if(close(fd) == -1)
	{
		perror("close");
		return 2;
	}

	return 0;
}


//int main(int argc, char *argv[], char* envp[])
//{
//	//Check if the arguments are valid
//	if(argc != 2)
//	{
//		char *usage = "Usage: sortrf <file name> \n";
//		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
//		{
//			perror("write");
//			return 1;
//		}
//		return 0;
//	}
//
//	return sortRfFile(argv[1]);
//}
