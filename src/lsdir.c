#include <unistd.h> //write
#include <sys/types.h> //opendir
#include <dirent.h> //opendir
#include <stdlib.h> //exit
#include <stdio.h> //perror
#include <sys/stat.h> //access permission macros
#include <fcntl.h> //open macros
#include <string.h> //strlen
#include <errno.h> //errno

#include "rfent.h"
#include "sizelim.h" //MAX_D_FULL_NAME

int main(int argc, char *argv[], char* envp[])
{
	//Step 1: Check if the arguments are valid
	if(argc != 3)
	{
		char *usage = "Usage: lsdir <dir> <file name> \n";
		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
		{
			perror("write");
			return 1;
		}
		return 0;
	}

	//Step 2: Open the file to store the regular file entries
	int fd;
	fd = open(argv[2], O_WRONLY|O_APPEND);

	if(fd == -1)
	{
		perror("open");
		return 1;
	}

	//Variables to process directory's contents
	DIR *dirp;
	struct dirent *direntp;
	struct stat stat_buf;
	struct rfent rf_buf;
	char name[MAX_D_FULL_NAME];

	//Step 3: Open the directory
	if((dirp = opendir(argv[1])) == NULL)
	{
		perror("opendir");
		close(fd);
		return 1;
	}

	//Step 4: Look for regular files
	while ((direntp = readdir(dirp)) != NULL)
	{
		//Step 4a: Check if the entry name is too large
		if(strlen(direntp->d_name) >= MAX_D_NAME)
		{
			write(STDERR_FILENO,direntp->d_name,strlen(direntp->d_name));
			char *name_too_large = " - name is too large (must be at most 128 including null terminator).\nIf its a regular file, it wont be processed.\n";
			write(STDERR_FILENO,name_too_large,strlen(name_too_large));
			continue;
		}

		//Step 4b: Check if the entry full name is too large
		if((strlen(argv[1]) + strlen(direntp->d_name) + 1)  >= MAX_D_FULL_NAME)
		{
			write(STDERR_FILENO,argv[1],strlen(argv[1]));
			write(STDERR_FILENO,"/",strlen("/"));
			write(STDERR_FILENO,direntp->d_name,strlen(direntp->d_name));
			char *name_too_large = " - full path name is too large (must be at most 385 including null terminator).\nIf its a regular file, it wont be processed.\n";
			write(STDERR_FILENO,name_too_large,strlen(name_too_large));
			continue;
		}

		//Step 4c: Retrieve directory entry's name
		if(sprintf(name,"%s/%s",argv[1],direntp->d_name) != (strlen(argv[1]) + strlen(direntp->d_name) + 1))
		{
			char* sprintf_error = "sprintf failed \n";
			write(STDOUT_FILENO,sprintf_error,strlen(sprintf_error));
			continue;
		}

		//Step 4d: Retrieve directory entry's info in stat_buf
		if(lstat(name, &stat_buf) == -1)
		{
			perror("lstat");
			continue;
		}

		//Step 4e: Check if the directory entry is a regular file
		if(S_ISREG(stat_buf.st_mode))
		{
			rf_buf.stat_buf = stat_buf;
			strcpy(rf_buf.d_name,direntp->d_name); //file name
			strcpy(rf_buf.d_full_name,name); //full path name

			if(write(fd,&rf_buf,sizeof(rf_buf)) == -1)
			{
				perror("write");
			}
		}
	}

	//Step 5: Close the directory
	if(closedir(dirp) == -1)
	{
		perror("closedir");
		close(fd);
		return 2;
	}

	//Step 6: Close the file
	if(close(fd) == -1)
	{
		perror("close");
		return 2;
	}

	return 0;
}
