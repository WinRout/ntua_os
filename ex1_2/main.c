#include <stdio.h>
#include <unistd.h>

#include "fun.h"

int main(int argc, char **argv)
{	
	if (argc <3 || argc >4) { //if arguments are wrong
		printf("Usage: ./fconc infile1 infile2 [outfile (default:fconc.out)]\n");
		return 1;
	}
	
	int fd, oflags, mode; //opening write-file...
	oflags = O_CREAT | O_WRONLY | O_TRUNC;
	mode = S_IRUSR | S_IWUSR;
	if (argc == 3) fd = open("fconc.out", oflags, mode); //default output file
	else           fd = open(argv[3], oflags, mode); //output file  determined by the usr
        
	write_file(fd, argv[1]);
	write_file(fd, argv[2]);

	close(fd); //closing file
}




