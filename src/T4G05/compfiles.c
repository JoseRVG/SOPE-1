#include <unistd.h> //read
#include <fcntl.h> //open macros
#include <stdio.h> //perror
#include <string.h> //strlen
#include <errno.h> //errno

#include "compfiles.h"

int compareFiles(char* filename1, char* filename2)
{
	int fd1, fd2; //file descriptors
	int res1, res2; //result of read for each file
	char p1[1]; //byte read from file1
	char p2[1]; //byte read from file2

	//Step 1: Open both files
	fd1 = open(filename1, O_RDONLY);
	if (fd1 == -1)
	{
		write(STDERR_FILENO, filename1, strlen(filename1));
		perror(" - open");
		return 2; //value indicating an error occured
	}

	fd2 = open(filename2, O_RDONLY);
	if (fd2 == -1)
	{
		close(fd1);
		write(STDERR_FILENO, filename2, strlen(filename2));
		perror(" - open");
		return 2;
	}

	//Step 2: Compare both files byte per byte
	while(res1 = read(fd1,p1,1), res1 == 1)
	{
		res2 = read(fd2,p2,1);
		switch(res2)
		{
		case -1: //failure on reading file2
			write(STDERR_FILENO, filename2, strlen(filename2));
			perror(" - read");
			close(fd2);
			close(fd1);
			return 2;
		case 0: //flle2 is shorter than file1
			close(fd2);
			close(fd1);
			return 0; //false = files differ in content
		}

		if(strncmp(p1, p2, 1) != 0) //file1 and file2 differ on a byte
		{
			close(fd2);
			close(fd1);
			return 0; //false
		}
	}
	if(res1==-1) //failure on reading file1
	{
		write(STDERR_FILENO, filename1, strlen(filename1));
		perror(" - read");
		close(fd2);
		close(fd1);
		return 2;
	}

	//Step 3: Check if file2 is larger than file1
	res2 = read(fd2,p2,1);
	switch(res2)
	{
	case -1: //failure on reading file2
		write(STDERR_FILENO, filename2, strlen(filename2));
		perror(" - read");
		close(fd2);
		close(fd1);
		return 2;
	case 1: //flle1 is shorter than file2
		close(fd2);
		close(fd1);
		return 0; //false
	}

	//Step 4: Close both files
	if(close(fd2) == -1)
	{
		write(STDERR_FILENO, filename2, strlen(filename2));
		perror(" - close");
		close(fd1);
		return 2;
	}

	if(close(fd1) == -1)
	{
		write(STDERR_FILENO, filename1, strlen(filename1));
		perror(" - close");
		return 2;
	}

	return 1; //true = files have the same content
}


//int main(int argc, char *argv[], char* envp[])
//{
//	//Check if the arguments are valid
//	if(argc != 3)
//	{
//		char *usage = "Usage: comprf <file name 1> <file name 2> \n";
//		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
//		{
//			perror("write");
//			return 1;
//		}
//		return 0;
//	}
//
//
//	int resp = compareFiles(argv[1],argv[2]);
//	if(resp == 1)
//	{
//		printf("They're the same!\n");
//	}
//	else if(resp == 0)
//	{
//		printf("They're different!\n");
//	}
//	else
//	{
//		printf("An error has ocorred.\n");
//	}
//
//	return 0;
//}
