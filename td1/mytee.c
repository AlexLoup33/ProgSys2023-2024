#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

#include "error.h"

//Write on the standard output
int main(int argc, char **argv)
{
  assert (argc == 2); // Nom du fichier de sortie attendu en ligne de commande
  char c;
  int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  check(fd != -1, "open failed");
  while(read(0, &c, 1)){
    printf("%c", c);
    write(fd, &c, 1);
  }
  close(fd);
  return EXIT_SUCCESS;
}