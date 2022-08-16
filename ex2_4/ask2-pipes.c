#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

void fork_procs(int fd, struct tree_node *root)
{
	/*
	 * Start
	 */
	printf("PID = %ld, name %s, starting...\n",
			(long)getpid(), root->name);
	change_pname(root->name);
	/*
	 *saving pid of children in an array
	 */
	 pid_t pid;
	 int status;
	 int pfd[2];
	 int i;
	 /*
	  *creating pipe
		*/
		if (pipe(pfd) < 0) {
			perror("pipe");
			exit(1);
		}
		for (i=0; i<root->nr_children; i++) {
			pid = fork();
			if(pid < 0) {
				perror("fork");
				exit(1);
			}
			else if (pid == 0) {
				// child is going to write to father, so fd = pfd[1]
				fork_procs(pfd[1], root->children + i);
			}
		}
		/*
		if not leaf, read from children // if leaf, skip for
		*/
		int ans[2], val;
		for (i=0; i < root->nr_children; i++)
		{
		printf("Node %s with PID: %ld. reading from child #%d.\n",
                root->name, (long)getpid(), i+1);
		/* father reads value from pipe */
		if (read(pfd[0], &val, sizeof(val)) != sizeof(val)) {
			perror("read from pipe");
			exit(1);
		}
		ans[i]= val;

		pid = wait(&status);
		explain_wait_status(pid, status);
	}
	/*
	compute value according to parent
	*/
	if ((root->name[0]) == '+') {
		val = ans[0] + ans[1];
	}
	else if ((root->name[0]) == '*') {
		val = ans[0] * ans[1];
	}
	else sscanf(root->name, "%d", &val);
	/*
	now write val to Father
	*/
	if (write(fd, &val, sizeof(val)) != sizeof(val)) {
		perror("write to pipe");
		exit(1);
	}

	/* exit */
			 printf("PID = %ld, name %s, exiting...\n",
											 (long)getpid(), root->name);
 exit(0);
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	int pfd[2];
	int ans;

	struct tree_node *root;

	if (argc < 2){
		fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
		exit(1);
	}

	/* Read tree into memory */
	root = get_tree_from_file(argv[1]);
	print_tree(root);

	//create pipe in int main(int argc, char const *argv[]) {
	if (pipe(pfd) < 0) {
	perror("main: pipe");
	exit(1);
	}
	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		fork_procs(pfd[1],root);
		exit(1);
	}
	/*
	 * Father
	 */
	 //read values from pipe
	 if (read(pfd[0], &ans, sizeof(ans)) != sizeof(ans)) {
                perror("read from pipe");
                exit(1);
	}
	wait(&status);
	explain_wait_status(pid, status);
	printf("computation: %d\n", ans);
	return 0;
}
