#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

int var1, var2;
void handler(int signum) {
  var1++;
  var2++;
}

void main() {
  int *A;
  int pfd[2];
  int status;
  pipe(pfd);


  A = (int*) malloc(16 * sizeof(int));
  A[0] = 0;

  var1 = getpid();
  var2 = fork();

  if (var2 == 0) {
    read(pfd[0], &var2, sizeof(int));

    while(1) {
      printf("child\n");
    }

    if (fork() == 0) {
      kill(var1, SIGUSR1);

      printf("parent of grandchild: %d\n", getppid());
      exit(0);
    }
    printf("child var1=%d, var2=%d\n", var1, var2);
    printf("child pid: %d\n", getpid());

    while(1) {
      printf("grandchild\n");
    }

    exit(0);
  }

  write(pfd[1], &var1, sizeof(int));
  signal(SIGUSR1, handler);

  printf("parent var1=%d var2=%d\n", var1, var2);
  while(1) {
    printf("parent");
  }
  A[0] = var1;
}
