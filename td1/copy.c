#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

#include "error.h"

int main(int argc, char **argv)
{
  assert(argc == 4);
  int input_file = open(argv[1], O_RDONLY);
  int output_file = open(argv[2], O_WRONLY|O_CREAT, 0666);

  int buffer_size = argv[3];
  char* buffer;
  
  int read_byte = 0;
  int ret_write = 0;

  do{
    read_byte = read(input_file, buffer, buffer_size);
    if (read_byte == 0){
      continue;
    }
    ret_write = write(output_file, buffer, buffer_size);
  }while (read_byte > 0);
  
  return EXIT_SUCCESS;
}
