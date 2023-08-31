#include <unistd.h> //read
#include <fcntl.h> //open macros
#include <string.h> //strcmp
#include <stdio.h> //perror
#include <stdlib.h> //malloc

#include "rfent.h"
#include "readrf.h"
#include "sizelim.h"

int readPerm(char *perms, mode_t mode)
{
	//Note: strlen(perms) must be 9!!
	if(strlen(perms) != 9)
	{
		return 2;
	}

	//owner read
	if(S_IRUSR & mode)
	{
		perms[0] = 'r';
	}
	else
	{
		perms[0] = '-';
	}

	//owner write
	if(S_IWUSR & mode)
	{
		perms[1] = 'w';
	}
	else
	{
		perms[1] = '-';
	}

	//owner execute
	if(S_IXUSR & mode)
	{
		perms[2] = 'x';
	}
	else
	{
		perms[2] = '-';
	}

	//group read
	if(S_IRGRP & mode)
	{
		perms[3] = 'r';
	}
	else
	{
		perms[3] = '-';
	}

	//group write
	if(S_IWGRP & mode)
	{
		perms[4] = 'w';
	}
	else
	{
		perms[4] = '-';
	}

	//group execute
	if(S_IXGRP & mode)
	{
		perms[5] = 'x';
	}
	else
	{
		perms[5] = '-';
	}

	//other read
	if(S_IROTH & mode)
	{
		perms[6] = 'r';
	}
	else
	{
		perms[6] = '-';
	}

	//other write
	if(S_IWOTH & mode)
	{
		perms[7] = 'w';
	}
	else
	{
		perms[7] = '-';
	}

	//other execute
	if(S_IXOTH & mode)
	{
		perms[8] = 'x';
	}
	else
	{
		perms[8] = '-';
	}

	return 0;
}

int readRFFile(char *raw_file_name, char *formatted_file_name)
{
	//Step 1: Open the raw file (binary file)
	int fd;
	fd = open(raw_file_name, O_RDONLY);

	if(fd == -1)
	{
		write(STDERR_FILENO, raw_file_name, strlen(raw_file_name));
		perror(" - open");
		return 2;
	}

	//Step 2: Open the formatted file
	int fd2;
	fd2 = open(formatted_file_name, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXU|S_IRWXU);

	if(fd2 == -1)
	{
		write(STDERR_FILENO, formatted_file_name, strlen(formatted_file_name));
		perror(" - open");
		close(fd);
		return 2;
	}

	//Step 3: Write the header to the file
	char header[MAX_RFENT_LINE];
	sprintf(header,"%30s | %30s | %10s | %10s | %11s | %10s | %10s\n",
			"file name","relative file path","size",
			"mode_t", "permissions",
			"inode num", "secs Epoch");

	if(write(fd2,header,strlen(header)) == -1)
	{
		perror("header write");
		close(fd2);
		close(fd);
		return 2;
	}


	//Variables to process the regular file entry's contents
	struct rfent rf_buf; //regular file entry read from the raw (binary) file
	char rf_entry[MAX_RFENT_LINE]; //corresponding string to be written to the .txt file

	//Step 4: Process the raw file
	int num_read; //number of bytes read from the raw file

	while ((num_read = read(fd,&rf_buf,sizeof(rf_buf))) == sizeof(rf_buf))
	{
		char perms[] = "---------";
		readPerm(perms,rf_buf.stat_buf.st_mode);

		//Retrieve regular file entry's relevant attributes
		sprintf(rf_entry,"%30s | %30s | %10d | %10d | %11s | %10d | %10d\n",
				rf_buf.d_name,rf_buf.d_full_name,(int)(rf_buf.stat_buf.st_size),
				(int)(rf_buf.stat_buf.st_mode), perms,
				(int)(rf_buf.stat_buf.st_ino), (int)(rf_buf.stat_buf.st_mtim.tv_sec));

		//Write out the regular file name
		if(write(fd2,rf_entry,strlen(rf_entry)) == -1)
		{
			perror("rfent write");
			close(fd2);
			close(fd);
			return 2;
		}
	}
	if(num_read == -1) //read failed
	{
		perror("raw file read");
		close(fd2);
		close(fd);
		return 2;
	}

	//Step 5: Close the formatted file
	if(close(fd2) == -1)
	{
		perror("formatted file close");
		close(fd);
		return 2;
	}

	//Step 6: Close the raw file
	if(close(fd) == -1)
	{
		perror("raw file close");
		return 2;
	}

	return 0;
}

//int main(int argc, char *argv[], char* envp[])
//{
//	//Check if the arguments are valid
//	if(argc != 3)
//	{
//		char *usage = "Usage: readrf <raw file name> <formatted file name> \n";
//		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
//		{
//			perror("write");
//			return 1;
//		}
//		return 0;
//	}
//
//	return readRFFile(argv[1],argv[2]);
//}
