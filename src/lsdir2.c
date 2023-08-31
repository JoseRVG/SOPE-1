//#include <unistd.h> //write
//#include <sys/types.h> //opendir
//#include <dirent.h> //opendir
//#include <stdlib.h> //exit
//#include <stdio.h> //perror
//#include <sys/stat.h> //access permission macros
//#include <fcntl.h> //open macros
//#include <string.h> //strlen
//
//#include "rfent2.h"
//
//int main(int argc, char *argv[], char* envp[])
//{
//	//Check if the arguments are valid
//	if(argc != 3)
//	{
//		char *usage = "Usage: lsdir <dir> <file name> \n";
//		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
//		{
//			perror("write");
//			exit(1);
//		}
//		exit(0);
//	}
//
//	//Open the file to store the regular file entries
//	int fd;
//	fd = open(argv[2], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXU|S_IRWXU);
//
//	if(fd == -1)
//	{
//		perror("open");
//		exit(2);
//	}
//
//	//Variables to process directory's contents
//	DIR *dirp;
//	struct dirent *direntp;
//	struct stat stat_buf;
//	struct rfent rf_buf;
//	char name[200];
//
//	//Open the directory
//	if((dirp = opendir(argv[1])) == NULL)
//	{
//		perror("opendir");
//		exit(3);
//	}
//
//	//Look for regular files
//	while ((direntp = readdir(dirp)) != NULL)
//	{
//		//Retrieve directory entry's name
//		if(sprintf(name,"%s/%s",argv[1],direntp->d_name) != (strlen(argv[1]) + strlen(direntp->d_name) + 1))
//		{
//			perror("sprintf");
//			exit(4);
//		}
//
//		//Retrieve directory entry's info in stat_buf
//		if(lstat(name, &stat_buf) == -1)
//		{
//			perror("lstat");
//			exit(5);
//		}
//
//		//Check if the directory entry is a regular file
//		if(S_ISREG(stat_buf.st_mode))
//		{
//			rf_buf.stat_buf = stat_buf;
//			strcpy(rf_buf.d_name,name);
//			if(write(fd,&rf_buf,sizeof(rf_buf)) == -1)
//			{
//				perror("write");
//				exit(6);
//			}
//		}
//	}
//
//	//Close the directory
//	if(closedir(dirp) == -1)
//	{
//		perror("closedir");
//		exit(7);
//	}
//
//	//Close the file
//	if(close(fd) == -1)
//	{
//		perror("close");
//		exit(8);
//	}
//
//	exit(0);
//}
