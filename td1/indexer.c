#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "error.h"

#define SUFFIXE ".idx"
#define BUF_SIZE 2048

void verifier(int cond, char *s){
  if (!cond){
    perror(s);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]){
  verifier(argc == 2, "il faut un paramètre.");

  // construire le chemin au fichier index
  int l = strlen(argv[1]) ;
  char idx_filename[l + strlen(SUFFIXE) + 1];
  strncpy(idx_filename, argv[1], l);
  strcpy(idx_filename + l, SUFFIXE);

  int file = open(argv[1],O_RDONLY);
  verifier(file != -1, argv[1]);
  
  int index = open(idx_filename, O_CREAT | O_WRONLY | O_TRUNC,0644);
  verifier(index != -1, idx_filename);

  char buffer[BUF_SIZE] ;
  off_t offset = 0;
  ssize_t rcount;
  do{
    rcount = read(file, buffer, BUF_SIZE);
    for (int i = 0; i < rcount; i++, offset++)
      if (buffer[i] == '\n')
	write(index, &offset, sizeof offset);
  } while (rcount > 0);
  close(file);
  close(index);

  return EXIT_SUCCESS;
}