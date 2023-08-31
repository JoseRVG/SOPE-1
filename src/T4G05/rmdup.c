#include <unistd.h> //fork, write, link
#include <sys/stat.h> //access permission macros
#include <fcntl.h> //open macros
#include <stdio.h> //perror
#include <string.h> //strlen
#include <sys/wait.h> //waitpid

#include "listrf.h" //listdirs
#include "sortrf.h" //sortRfFile
#include "readrf.h" //readRFFile
#include "sizelim.h" //MAX_RFENTS

#define FILES_BIN  "files"
#define FILES_TXT  "files.txt"
#define HLINKS_TXT "hlinks.txt"
#define NUM_FILES 3

int clearFiles(char *filenames[], int num_files)
{
	int i;
	for(i = 0; i < num_files; i++)
	{
		int fd = open(filenames[i], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXU|S_IRWXU);
		if (fd == -1)
		{
			perror("clearFiles - open");
			return 2; //"2" stands for a "fatal error" that stops the program from working as expected (thus, the program must end)
		}
		if(close(fd) == -1)
		{
			perror("clearFiles - close");
			return 2;
		}
	}
	return 0;
}

int main(int argc, char *argv[], char* envp[])
{
	//Step 1: Check if the arguments are valid
	if(argc != 2)
	{
		char *usage = "Usage: rmdup <dir> \n";
		if(write(STDOUT_FILENO,usage,strlen(usage)) == -1)
		{
			perror("write");
			return 1;
		}
		return 0;
	}

	//Step 2: Clear all the files to be used
	char *filenames[] = {FILES_BIN, FILES_TXT, HLINKS_TXT}; //array of the names of the files used by the program
	int return_value = clearFiles(filenames,NUM_FILES); //return value of several function calls
	if(return_value == 2)
	{
		char *clearFiles_fail = "clearFiles failed \n";
		write(STDERR_FILENO,clearFiles_fail,strlen(clearFiles_fail));
		return 2;
	}

	//Step 3: List all regular files in names (binary file)
	return_value = listdirs(argv[1],FILES_BIN);
	if(return_value == 2)
	{
		write(STDERR_FILENO,argv[1],strlen(argv[1]));
		char *listrf_fail = " - listdirs failed \n";
		write(STDERR_FILENO,listrf_fail,strlen(listrf_fail));
		return 2;
	}

	//Step 4: Sort out the regular file entries of names
	struct rfent entries[MAX_RFENTS]; //all regular file entries
	unsigned int num_entries = 0; //current size of the entries array
	return_value = sortRfFile(FILES_BIN,entries,&num_entries,MAX_RFENTS);
	if(return_value == 2)
	{
		char *sortrf_fail = "sortRfFile failed \n";
		write(STDERR_FILENO,sortrf_fail,strlen(sortrf_fail));
		return 2;
	}

	//Step 5: Write all the regular files to names.txt
	return_value = readRFFile(FILES_BIN,FILES_TXT);
	if(return_value == 2)
	{
		char *readrf_fail = "readRFFile failed \n";
		write(STDERR_FILENO,readrf_fail,strlen(readrf_fail));
		return 2;
	}

	//Step 6: Look for duplicate files
	unsigned int older_entries[MAX_RFENTS]; //entries' index on the entries array that are mutually shallow equal but not deep equals
	unsigned int num_oe = 0; //number of "slots" currently being used in the older_entries array

	//Step 6a: Open the hlinks.txt file
	int fd;
	fd = open(HLINKS_TXT,O_WRONLY|O_APPEND);
	if(fd == -1)
	{
		perror("HLINKS_TXT open");
		return 2;
	}

	//Step 6b: Look for duplicate files
	unsigned int i;
	for(i = 0; i < num_entries; i++)
	{
		struct rfent newer_entry = entries[i];
		if(i == 0)
		{
			older_entries[num_oe] = i;
			num_oe++;
			continue;
		}
		if(shallow_equals(entries[older_entries[0]],newer_entry))
		{
			int new_oe = 1; //new rfent is potentially equal to an element of older_entries
			unsigned int j;
			for(j = 0; j < num_oe; j++)
			{
				struct rfent older_entry = entries[older_entries[j]];
				int equals_res = equals(older_entry,newer_entry);
				if(equals_res == 1) //newer_entry is equal to an older_entry
				{
					//Create a safety copy
					char safe_copy[MAX_D_NAME + 5];
					sprintf(safe_copy,"/tmp/%s",newer_entry.d_name);

					if(link(newer_entry.d_full_name,safe_copy) == -1)
					{
						write(STDERR_FILENO,safe_copy,strlen(safe_copy));
						perror(" - link for a safety copy");
					}

					//Erase the copy
					if(unlink(newer_entry.d_full_name) == -1)
					{
						write(STDERR_FILENO,newer_entry.d_full_name,strlen(newer_entry.d_full_name));
						perror(" - unlink");
						close(fd);
						return 2;
					}

					//Create the hard link
					if(link(older_entry.d_full_name,newer_entry.d_full_name) == -1)
					{
						write(STDERR_FILENO,older_entry.d_full_name,strlen(older_entry.d_full_name));
						perror(" - link");
						close(fd);
						return 2;
					}

					//Erase the safe copy
					if(unlink(safe_copy) == -1)
					{
						write(STDERR_FILENO,safe_copy,strlen(safe_copy));
						perror(" - unlink of a safety copy");
					}

					//Register the copy at hlinks.txt
					char str[2*MAX_D_FULL_NAME + 100];
					sprintf(str,"%s (inode: %d) -> %s (inode: %d)\n",newer_entry.d_full_name,(int)newer_entry.stat_buf.st_ino,older_entry.d_full_name,(int)older_entry.stat_buf.st_ino);
					if(write(fd,str,strlen(str)) == -1)
					{
						write(STDERR_FILENO,str,strlen(str));
						perror(" - write");
					}
					new_oe = 0;
					break;
				}
				else if(equals_res == 2) //failure (non fatal) on comparing files.
				{
					new_oe = 0; //older entries are not to be discarded
				}
			}
			if(new_oe)
			{
				older_entries[num_oe] = i;
				num_oe++;
			}
		}
		else //since the entries are sorted, finding a "shallow different" rfent enables us to discard all oe's
		{
			num_oe = 0;
			older_entries[num_oe] = i;
			num_oe++;
		}
	}

	//Close the hlinks.txt file
	if(close(fd) == -1)
	{
		perror("H_LINKS close");
		return 2;
	}
	return 0;
}
