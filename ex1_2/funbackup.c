#include "fun.h"

void doWrite(int fd, const char *buff, int len) 
{
	if(buff == NULL) return;;
	size_t idx = 0;
	ssize_t wcnt;
	do {
	   wcnt = write(fd, buff+idx, len-idx);
	   if(wcnt == -1) {
		perror("write");
		exit(1);
	   }
	   idx += wcnt;
	} while (idx < len);
}

void write_file(int fd, const char *infile) {
	
	if (fd == -1) {
		perror("write");
		exit(1);
	}
	int fdo; //opening read-file...
	fdo = open(infile, O_RDONLY);
	if (fdo == -1) {
		perror(infile);
		exit(1);
	}
	
	char buff[1024]; //reading file...
	size_t idx=0;
	ssize_t rcnt;
	rcnt = read(fdo, buff+idx, sizeof(buff)-1-idx);
	if (rcnt == 0) //means end of file
		return;
	if (rcnt == -1) {
		perror("read");
		exit(1);
	}
	
	buff[rcnt] = '\0';//putting end-of-file char in buffer
	close(fdo); //closing read-file
	doWrite(fd,buff,strlen(buff)); //writing to write-file
}


		

		

