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

void verifier(int cond, char *s){
  if (!cond){
    perror(s);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]){
  verifier(argc == 3, "il faut deux paramètres.");

  // construire le chemin au fichier index
  int l = strlen(argv[1]) ;
  char idx_filename[l + strlen(SUFFIXE) + 1];

  strncpy(idx_filename, argv[1], l);
  strcpy(idx_filename + l, SUFFIXE);

  FILE *file = fopen(argv[1],"r");
  verifier(file != NULL, argv[1]);
  
  FILE *index = fopen(idx_filename, "r");
  verifier(index != NULL, idx_filename);
		   
  int numero = atoi(argv[2]);
  

  if (numero > 1){
    off_t offset;
    fseek(index, (numero - 2) * sizeof offset, SEEK_SET);

    int rcount = fread(&offset, sizeof offset, 1, index);
    verifier(rcount == 1, "pas assez de lignes");

    offset++;
    fseek(file,offset,SEEK_SET);
  }
  
  char c;

  while(fread(&c,1,1,file) == 1 && c != '\n')
    fwrite(&c,1,1,stdout);
  if( c == '\n')
    fwrite("\n",1,1,stdout);
  return EXIT_SUCCESS;
}
