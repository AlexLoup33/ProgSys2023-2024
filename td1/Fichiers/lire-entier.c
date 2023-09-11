#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#define INTSIZE 64

void
perror_and_exit_whenever(int assertion)
{
  if (assertion)
    {
      perror("");
      exit(EXIT_FAILURE);
    }
}


int
main(int argc, char **argv)
{
  int input = open(argv[1], O_RDONLY);
  int position = strtoul(argv[2], NULL, 10);
  int valeur  ;


  perror_and_exit_whenever(input < 0);

  lseek(input, position, SEEK_SET);
  
  read(input,&valeur,sizeof valeur);
  
  printf("%d\n",valeur);

  close(input);

  return EXIT_SUCCESS;
}