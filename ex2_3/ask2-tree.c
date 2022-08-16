#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

void
procs_tree(struct tree_node *root, int level, int exit_offset)
{
  printf("%s started...\n", root->name);
  change_pname(root->name);

  if (root->nr_children >0) {
    printf("%s waiting %d children...\n", root->name, root->nr_children);
    pid_t p;
    int status;
    int i;
    for(i=0; i<root->nr_children; i++){
      p=fork();
      if (p<0) {
        perror("fork");
        exit(1);
      }
      if (p==0) {
          procs_tree(root->children + i, level+1, i);
      }
    }
    int j;
    for(j=0; j<root->nr_children; j++){
      p=wait(&status);
      explain_wait_status(p, status);
    }
  }
  else {
    printf("%s sleeping...\n", root->name);
    sleep(SLEEP_PROC_SEC);
  }
  int exit_n = level*10 + exit_offset;
  printf("%s exiting...\n", root->name);
  exit(exit_n);
}

int main(int argc, char *argv[])
{
	struct tree_node *root;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
		exit(1);
	}
	root = get_tree_from_file(argv[1]);
  printf("File Tree:\n");
  print_tree(root);

	pid_t pid;
	int status;

	pid=fork();
	if (pid<0) {
 		perror("fork");
		exit(1);
	}
	if (pid==0) {
    printf("\nCreating Proccess Tree...\n");
    procs_tree(root, 0, 0);
	}
  sleep(SLEEP_TREE_SEC);
  printf("\nProccess Tree:\n");
	show_pstree(pid);
  pid=wait(&status);
  explain_wait_status(pid, status);
	return 0;
}
