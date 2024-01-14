/***********
Authors: AlexLoup33
Date: 14-01-2024
Description: Recoding of the command ls and the argument -R for reccursive
***********/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <dirent.h>
#include <stdbool.h>

#define FILE_MAX 256
#define DT_DIR 4

//Check if the condition is true, and exit the program with a message if not
void verifier(bool condition, char* message){
    if (!condition){
        perror(message);
        exit(EXIT_FAILURE);
    }
}

//Sort of tab of char* in alphabetic order
char** sort_tab(char** t, int size){
    for (int i = 0; i < size; i++){
        for (int j = i+1; j < size; j++){
            if (strcmp(t[i], t[j]) > 0){
                char* temp = t[i];
                t[i] = t[j];
                t[j] = temp;
            }
        }
    }
    return t;
}

//Print the name of the current dir
void print_dir_name(){
    char pwd[FILE_MAX];
    if (getcwd(pwd, sizeof(pwd)) != NULL){
        printf("%s\n", pwd);
    }else{
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
}

//Check if the name given in parameter is a file or not
bool is_file(char* name){
    struct stat st;
    verifier(stat(name, &st) != -1, "ls: cannot access 'file_not_found'");
    return (S_ISDIR(st.st_mode));
}

//Create a new word by adding c1 to c2 given in parameter
char* concat(const char* c1, const char* c2){
    char* c = malloc(strlen(c1) + strlen(c2) + 2);
    if (c == NULL){
        fprintf(stderr, "Memory not allocated\n");
        exit(EXIT_FAILURE);
    }
    strcpy(c, c1);
    strcat(c, c2);
    return c;
}

//Simple ls
void read_dir(char* name){
    DIR* folder;
    struct dirent* entry;
    char** file_list = malloc(FILE_MAX * sizeof(char*));
    int file_ind = 0;

    folder = opendir(name);
    verifier(folder != NULL, "ls: cannot access 'file_not_found'");

    if ((entry = readdir(folder)) == NULL){
        printf("ls: cannot access '%s': No such file or directory\n", name);
        exit(EXIT_FAILURE);
    }
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
        file_list[file_ind] = entry->d_name;
        file_ind++;
    }
    while((entry = readdir(folder)) != NULL){
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            file_list[file_ind] = entry->d_name;
            file_ind++;
        }
    }
    sort_tab(file_list, file_ind);
    for (int i = 0; i < file_ind; i++){
        printf("%s\n", file_list[i]);
    }
    closedir(folder);
}

//Reccursive ls: ls -R
void rec_read_dir(char *d){
    DIR *folder;
    struct dirent *entry;

    int file_ind = 0;
    char** file_list = malloc(FILE_MAX * sizeof(char*));
    verifier(file_list != NULL, "Not enought memory to allocate");

    int dir_ind = 0;
    char** dir_list = malloc(FILE_MAX * sizeof(char*));
    verifier(dir_list != NULL, "Not enought memory to allocate");

    folder = opendir(d);
    verifier(folder != NULL, "ls: cannot access 'file_not_found'");

    if ((entry = readdir(folder)) == NULL){
        printf("ls: cannot access '%s': No such file or directory\n", d);
        exit(EXIT_FAILURE);
    }
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
        if (entry->d_type != DT_DIR){
            file_list[file_ind] = entry->d_name;
            file_ind++;
        }else{
            dir_list[dir_ind] = entry->d_name;
            dir_ind++;
        }
    }
    while((entry = readdir(folder)) != NULL){
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            if (entry->d_type != DT_DIR){
                file_list[file_ind] = entry->d_name;
                file_ind++;
            }else{
                dir_list[dir_ind] = entry->d_name;
                dir_ind++;
            }
        }
    }
    sort_tab(file_list, file_ind);
    sort_tab(dir_list, dir_ind);

    printf("%s:\n", d);
    read_dir(d);

    for(int i = 0; i < dir_ind; i++){
        char* dir_name = concat(d, "/");
        dir_name = concat(dir_name, dir_list[i]);
        printf("\n");
        rec_read_dir(dir_name);
        free(dir_name);
    }
}

//Main function
int main(int argc, char *argv[]){
    if (argc == 1){
        read_dir(".");
        return (EXIT_SUCCESS);
    }
    if (argc == 2 && strcmp(argv[1], "-R") == 0){
        rec_read_dir(".");
        return (EXIT_SUCCESS);
    }
    int R_ind = 0;
    bool rec = false;
    char* file_list[argc];
    int file_ind = 0;
    char* dir_list[argc];
    int dir_ind = 0;

    //Check if the parameters contain "-R" and separate dir and file in array.
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-R") == 0){
            rec = true;
            R_ind = i;
        }
        if (R_ind !=i){
            if (!is_file(argv[i])){
                file_list[file_ind] = argv[i];
                file_ind++;
            }
            else{
                dir_list[dir_ind] = argv[i];
                dir_ind++;
            }
        }
    }
    
    //Sort the dir and the file array
    sort_tab(file_list, file_ind);
    sort_tab(dir_list, dir_ind);

    if (!rec){
        if (argc == 1 && dir_ind == 0 && file_ind == 0){
            char pwd[FILE_MAX];
            getcwd(pwd, sizeof(pwd));
            printf("%s\n", pwd);
            opendir(pwd);
        } else if (argc == 1 && dir_ind == 0 && file_ind != 0){
            for (int i = 0; i < file_ind; i++){
                printf("%s\n", file_list[i]);
            }
        }
        for (int x = 0; x < dir_ind; x++){
            char* dir_name = NULL;
            if (dir_ind > 1){
                dir_name = malloc(strlen(dir_list[x]) + strlen("/:") + 1);
                strcpy(dir_name, dir_list[x]);
                strcat(dir_name, ":");
                printf("%s\n", dir_name); 
            }
            read_dir(dir_list[x]);
            if (dir_name == NULL) free(dir_name);
            if (x != dir_ind -1 && dir_ind > 1) printf("\n");
        }
        for (int y = 0; y < file_ind; y++){
            printf("%s\n", file_list[y]);
        }
    }
    else{
        for (int y = 0; y < file_ind; y++){
            printf("%s\n\n", file_list[y]);
        }
        for (int x = 0; x < dir_ind; x++){
            rec_read_dir(dir_list[x]);
        }
    }
    return EXIT_SUCCESS;
}