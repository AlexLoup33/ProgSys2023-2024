#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

#include "error.h"

void quelques_prints (void)
{
  printf ("Juste");
  printf (" histoire");
  printf (" de tester...\n");
  printf ("...que la redirection");
  printf (" marche !\n");
}

void rediriger_vers (void (*f)(void), char *file){
  int stdout_backup = dup (STDOUT_FILENO);
  int fd = open(file, O_CREAT|O_TRUNC|O_WRONLY, 0666);

  dup2(fd, STDOUT_FILENO);
  close(fd);

  f();

  dup2(stdout_backup, STDOUT_FILENO);
  close(stdout_backup);
}

int main(int argc, char *argv[])
{
  printf ("*** DEBUT ***\n");

  rediriger_vers (quelques_prints, "sortie.txt");

  printf ("*** FIN ***\n");

  return EXIT_SUCCESS;
}
  
