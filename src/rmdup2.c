//#include <unistd.h> //fork, write
//#include <sys/stat.h> //access permission macros
//#include <fcntl.h> //open macros
//#include <stdio.h> //perror
//#include <string.h> //strlen
//#include <sys/wait.h> //waitpid
//
//#include "listrf.h" //listdirs
//#include "sortrf.h" //sortRfFile
//#include "readrf.h" //readRFFile
//#include "comprf.h" //compFiles
//
//int main(int argc, char *argv[], char* envp[])
//{
//	//Check if the arguments are valid
//	if(argc != 2)
//	{
//		char *usage = "Usage: rmdup <dir> \n";
//		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
//		{
//			perror("write");
//			return 1;
//		}
//		return 0;
//	}
//
//	//Write the header on names.txt
//	int fd;
//	fd = open("names.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXU|S_IRWXU);
//
//	if(fd == -1)
//	{
//		perror("open");
//		return 2;
//	}
//
//	//List all regular files in names.txt
//	int return_value = listdirs(argv[1],"names.txt");
//	if(return_value != 0)
//	{
//		return 3;
//	}
//
//	//Sort out the regular file entries of names.txt
//	pid_t pid = fork();
//	char *exec_error = "Failed to execute execlp. \n";
//
//	switch(pid)
//	{
//	case -1: //error
//		perror("fork");
//		return 4;
//	case 0: //child process
//		dup2(fd,STDOUT_FILENO);
//		execlp("sort","sort","names.txt",NULL);
//		write(STDOUT_FILENO,exec_error,strlen(exec_error));
//		return 5;
//	}
//
//	//Retrieve the child process' return code
//	int status;
//	waitpid(pid,&status,0);
//
//	//Child ended abnormally
//	if(!(WIFEXITED(status)))
//	{
//		return 6;
//	}
//
//	//Add the header to	 names.txt
//	char *header = "1iname\tsize(in bytes)\tmode_t int\tlast modification time\tinode number\tcomplete path name\n";
//	pid = fork();
//
//	switch(pid)
//	{
//	case -1: //error
//		perror("fork");
//		return 7;
//	case 0: //child process
//		dup2(fd,STDOUT_FILENO);
//		execlp("sed","sed","-i",header,"names.txt",NULL); //sed -i '1istring to append to the begginng' names.txt
//		write(STDOUT_FILENO,exec_error,strlen(exec_error));
//		return 8;
//	}
//
//	//Retrieve the child process' return code
//	waitpid(pid,&status,0);
//
//	//Child ended abnormally
//	if(!(WIFEXITED(status)))
//	{
//		return 9;
//	}
//
//	//Close the file
//	if(close(fd) == -1)
//	{
//		perror("close");
//		return 10;
//	}
//
//	return 0;
//}
