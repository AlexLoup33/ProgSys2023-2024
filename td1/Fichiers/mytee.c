#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

#include "error.h"

int main(int argc, char **argv)
{
  assert (argc == 2); // Nom du fichier de sortie attendu en ligne de commande

  //....

  return EXIT_SUCCESS;
}
