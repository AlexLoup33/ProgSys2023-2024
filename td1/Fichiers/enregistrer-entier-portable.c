#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <netinet/in.h>

void
perror_and_exit_whenever(int assertion){
  if (assertion){
      perror("");
      exit(EXIT_FAILURE);
    }
}


int main(int argc, char **argv){
  int output = open(argv[1], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
  int position = strtoul(argv[2], NULL, 10)*sizeof(uint32_t);
  uint32_t valeur  = htonl(strtoul(argv[3], NULL, 10));


  perror_and_exit_whenever(output < 0);

  lseek(output, position, SEEK_SET);
  
  write(output,&valeur,sizeof valeur);
  
  close(output);

  return EXIT_SUCCESS;
}

