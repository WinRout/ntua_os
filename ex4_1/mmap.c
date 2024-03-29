/*
 * mmap.c
 *
 * Examining the virtual memory of processes.
 *
 * Operating Systems course, CSLab, ECE, NTUA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>

#include "help.h"

#define RED     "\033[31m"
#define RESET   "\033[0m"


char *heap_private_buf;
char *heap_shared_buf;

char *file_shared_buf;

uint64_t buffer_size;


/*
 * Child process' entry point.
 */
void child(void)
{
	uint64_t pa;

	/*
	 * Step 7 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 7.
	 */
	 printf("Child's VM Map\n");
	 show_maps();

	/*
	 * Step 8 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 8.
	 */
	 uint64_t paddr;
	 paddr = get_physical_address((uint64_t)heap_private_buf);
	 printf("Physical addr in child: %ld\n", paddr);

	/*
	 * Step 9 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 9.
	 */
	 memset(heap_private_buf, 0, buffer_size);
	 paddr = get_physical_address((uint64_t)heap_private_buf);
	 printf("Physical addr in child: %ld\n", paddr);

	/*
	 * Step 10 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 10.
	 */
	 memset(heap_shared_buf, 0, buffer_size);
	 paddr = get_physical_address((uint64_t)heap_shared_buf);
	 printf("Physical addr in child: %ld\n", paddr);

	/*
	 * Step 11 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 11.
	 */
	 mprotect(heap_shared_buf,buffer_size,PROT_READ);
         printf("Child's VM map:\n");
         show_va_info(heap_shared_buf);

	/*
	 * Step 12 - Child
	 */
	/*
	 * TODO: Write your code here to complete child's part of Step 12.
	 */
	 //munmap(heap_shared_buf,buffer_size);
	 munmap(heap_private_buf,buffer_size);
	 //munmap(file_shared_buf,buffer_size);
}

/*
 * Parent process' entry point.
 */
void parent(pid_t child_pid)
{
	uint64_t pa;
	int status;

	/* Wait for the child to raise its first SIGSTOP. */
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");

	/*
	 * Step 7: Print parent's and child's maps. What do you see?
	 * Step 7 - Parent
	 */
	printf(RED "\nStep 7: Print parent's and child's map.\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 7.
	 */
	 printf("Parent's VM Map\n");
	 show_maps();

	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");


	/*
	 * Step 8: Get the physical memory address for heap_private_buf.
	 * Step 8 - Parent
	 */
	printf(RED "\nStep 8: Find the physical address of the private heap "
		"buffer (main) for both the parent and the child.\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 8.
	 */
	 uint64_t paddr;
	 paddr = get_physical_address((uint64_t)heap_private_buf);
	 printf("Physical addr in parent: %ld\n", paddr);

	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");


	/*
	 * Step 9: Write to heap_private_buf. What happened?
	 * Step 9 - Parent
	 */
	printf(RED "\nStep 9: Write to the private buffer from the child and "
		"repeat step 8. What happened?\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 9.
	 */
	 paddr = get_physical_address((uint64_t)heap_private_buf);
	 printf("Physical addr in child: %ld\n", paddr);


	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");


	/*
	 * Step 10: Get the physical memory address for heap_shared_buf.
	 * Step 10 - Parent
	 */
	printf(RED "\nStep 10: Write to the shared heap buffer (main) from "
		"child and get the physical address for both the parent and "
		"the child. What happened?\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 10.
	 */
	 paddr = get_physical_address((uint64_t)heap_shared_buf);
	 printf("Physical addr in parent: %ld\n", paddr);


	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");


	/*
	 * Step 11: Disable writing on the shared buffer for the child
	 * (hint: mprotect(2)).
	 * Step 11 - Parent
	 */
	printf(RED "\nStep 11: Disable writing on the shared buffer for the "
		"child. Verify through the maps for the parent and the "
		"child.\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 11.
	 */
	 printf("Parent's VM map:\n");
	 show_va_info(heap_shared_buf);


	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, 0))
		die("waitpid");


	/*
	 * Step 12: Free all buffers for parent and child.
	 * Step 12 - Parent
	 */

	/*
	 * TODO: Write your code here to complete parent's part of Step 12.
	 */
	  munmap(heap_shared_buf,buffer_size);
    //munmap(heap_private_buf,buffer_size);
    //munmap(file_shared_buf,buffer_size);
}

int main(void)
{
	pid_t mypid, p;
	int fd = -1;
	uint64_t pa;

	mypid = getpid();
	buffer_size = 1 * get_page_size();

	/*
	 * Step 1: Print the virtual address space layout of this process.
	 */

	printf(RED "\nStep 1: Print the virtual address space map of this "
		"process [%d].\n" RESET, mypid);
	press_enter();
	/*
	 * TODO: Write your code here to complete Step 1.
	 */
	 show_maps();

	/*
	 * Step 2: Use mmap to allocate a buffer of 1 page and print the map
	 * again. Store buffer in heap_private_buf.
	 */
	printf(RED "\nStep 2: Use mmap(2) to allocate a private buffer of "
		"size equal to 1 page and print the VM map again.\n" RESET);
	press_enter();
	/*
	 * TODO: Write your code here to complete Step 2.
	 */
	 pa = mmap(NULL, buffer_size, (PROT_READ|PROT_WRITE), MAP_PRIVATE|MAP_ANONYMOUS, fd, 0);
	 if (pa == MAP_FAILED){
		 printf("Error in mapping");
		 return 1;
	 }
	 show_maps();
	 show_va_info((uint64_t)pa);
	 heap_private_buf = pa;

	/*
	 * Step 3: Find the physical address of the first page of your buffer
	 * in main memory. What do you see?
	 */
	printf(RED "\nStep 3: Find and print the physical address of the "
		"buffer in main memory. What do you see?\n" RESET);
	press_enter();
	/*
	 * TODO: Write your code here to complete Step 3.
	 */
	get_physical_address((unsigned long)pa);

	/*
	 * Step 4: Write zeros to the buffer and repeat Step 3.
	 */
	printf(RED "\nStep 4: Initialize your buffer with zeros and repeat "
		"Step 3. What happened?\n" RESET);
	press_enter();
	/*
	 * TODO: Write your code here to complete Step 4.
	 */

	 //uint64_t paddr;
	 memset(pa, 0, buffer_size);
	 //paddr =
	 printf("Physical Address: %ld\n",get_physical_address((uint64_t)pa));
	 //printf("Physical addr: %ld", paddr);

	/*
	 * Step 5: Use mmap(2) to map file.txt (memory-mapped files) and print
	 * its content. Use file_shared_buf.
	 */
	printf(RED "\nStep 5: Use mmap(2) to read and print file.txt. Print "
		"the new mapping information that has been created.\n" RESET);
	press_enter();
	/*
	 * TODO: Write your code here to complete Step 5.
	 */

	 //open file:
	 int fileno = open("file.txt", O_RDONLY);
	 if (fileno == -1) {
		 printf("error in opening file");
		 return 1;
	 }
	 //find size of file:
	 struct stat buf;
	 fstat(fileno, &buf);
	 int size = buf.st_size;
	 //allocate VM:
	 file_shared_buf = mmap(NULL, size, PROT_READ,  MAP_PRIVATE, fileno, 0);
	 if (file_shared_buf == MAP_FAILED){
		 printf("Error in mapping");
		 return 1;
	 }
	 //print VM map:
	 show_maps();
	 //print file content from VM map:
	 int i;
	 for(i=0; i<size; i++){
		 	char c;
			c = file_shared_buf[i];
			printf("%c",c);
	 }
	/*
	 * Step 6: Use mmap(2) to allocate a shared buffer of 1 page. Use
	 * heap_shared_buf.
	 */
	printf(RED "\nStep 6: Use mmap(2) to allocate a shared buffer of size "
		"equal to 1 page. Initialize the buffer and print the new "
		"mapping information that has been created.\n" RESET);
	press_enter();
	/*
	 * TODO: Write your code here to complete Step 6.
	 */
		heap_shared_buf = mmap(NULL, buffer_size, (PROT_READ|PROT_WRITE), MAP_SHARED|MAP_ANONYMOUS, fd, 0);
		if (heap_shared_buf == MAP_FAILED){
			printf("Error in mapping");
			return 1;
		}
		show_maps();
		show_va_info((uint64_t)heap_shared_buf);
	memset(heap_shared_buf, 0, buffer_size); 

	p = fork();
	if (p < 0)
		die("fork");
	if (p == 0) {
		child();
		return 0;
	}

	parent(p);

	if (-1 == close(fd))
		perror("close");
	return 0;
}
