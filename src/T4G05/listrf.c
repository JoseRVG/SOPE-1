#include <unistd.h> //fork, write
#include <sys/types.h> //opendir
#include <dirent.h> //opendir
#include <sys/stat.h> //lstat
#include <sys/wait.h> //waitpid
#include <string.h> //strlen, strcmp
#include <stdio.h> //perror
#include <errno.h> //errno

#include "listrf.h"
#include "sizelim.h" //MAX_DIR_NAME

int listdirs(char *dir_name, char *file_name)
{
	//Variables to process directory's contents
	DIR *dirp; //pointer to a directory
	struct dirent *direntp; //contains the info of a directory entry
	struct stat stat_buf; //contains info obtained by lstat
	char name[MAX_DIR_NAME]; //string to store the directory's name (relative to the directory given as arg to rmdup)


	//Step 1: Open the directory
	if((dirp = opendir(dir_name)) == NULL)
	{
		write(STDERR_FILENO,dir_name,strlen(dir_name));
		perror(" - opendir");
		return 1;
	}

	//Step 2: Process the regular files of the directory
	pid_t pid = fork();
	char *exec_error = " - Failed to execute execlp.\n";

	switch(pid)
	{
	case -1: //error
		write(STDERR_FILENO,dir_name,strlen(dir_name));
		perror(" - fork");
		closedir(dirp);
		return 1;
	case 0: //child process
		execl("./bin/lsdir","lsdir",dir_name,file_name,NULL);
		write(STDERR_FILENO,dir_name,strlen(dir_name));
		write(STDERR_FILENO,exec_error,strlen(exec_error));
		closedir(dirp);
		return 1;
	}
	//father process

	//Step 3: Look for child directories
	while ((direntp = readdir(dirp)) != NULL)
	{
		//Step 3a: Check if the directory entry's name is too large (if it is a directory)
		if(strlen(dir_name) + strlen(direntp->d_name) + 1 >= MAX_DIR_NAME)
		{
			write(STDERR_FILENO,direntp->d_name,strlen(direntp->d_name));
			char *name_too_large = " - name is too large (must be at most 256 including null terminator).\nIf its a directory, it wont be processed.\n";
			write(STDERR_FILENO,name_too_large,strlen(name_too_large));
			continue;
		}

		//Step 3b: Retrieve directory entry's name
		if(sprintf(name,"%s/%s",dir_name,direntp->d_name) != (strlen(dir_name) + strlen(direntp->d_name) + 1))
		{
			write(STDERR_FILENO,dir_name,strlen(dir_name));
			char *sprintf_error = " - sprintf failed.\n";
			write(STDERR_FILENO,sprintf_error,strlen(sprintf_error));
			continue;
		}

		//Step 3c: Retrieve directory entry's info in stat_buf
		if(lstat(name, &stat_buf) == -1)
		{
			write(STDERR_FILENO,dir_name,strlen(dir_name));
			perror(" - lstat");
			continue;
		}

		//Step 3d: Check if the diretory entry is another directory
		if(S_ISDIR(stat_buf.st_mode))
		{
			//Check if the directory is neither the current (..) or the previous (.)
			if(strcmp("..",direntp->d_name) != 0 && strcmp(".",direntp->d_name) != 0)
			{
				//If so, recursively call this function for that directory
				int return_value = listdirs(name,file_name);
				if(return_value != 0)
				{
					write(STDERR_FILENO,name,strlen(dir_name));
					char *listrf_fail = " - listdirs failed \n";
					write(STDERR_FILENO,listrf_fail,strlen(listrf_fail));
					if(return_value == 2) //error for which it is wiser to end the program
					{
						closedir(dirp);
						waitpid(pid,NULL,0);
						return 2;
					}
				}
			}
		}
	}

	//Step 4: Close the directory
	if(closedir(dirp) == -1)
	{
		write(STDERR_FILENO,name,strlen(dir_name));
		perror(" - closedir");
		return 2;
	}

	//Step 5: Retrieve the child process' return code
	int status;
	waitpid(pid,&status,0);

	//Child ended normally
	if(WIFEXITED(status))
	{
		return 0;
	}
	else //child ended abnormally
	{
		return 1;
	}
}


//int main(int argc, char *argv[], char* envp[])
//{
//	//Check if the arguments are valid
//	if(argc != 3)
//	{
//		char *usage = "Usage: listrf <base dir> <file name> \n";
//		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
//		{
//			perror("write");
//			return 1;
//		}
//		return 0;
//	}
//
//	return listdirs(argv[1],argv[2]);
//}
