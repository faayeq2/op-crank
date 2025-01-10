#include <stdio.h>
#include<string.h>
#include<stdlib.h>

void tokenise_line(const char* line){
   
}

int main(int argc, char *argv[]) {
    if(argc!=2){
        printf("Using: %s <f_name>\n, argv[0]");
        return 1;
    }
    const char* f_name = argv[1];

    FILE *file = fopen(f_name, "r");
    if(file==NULL){
       perror("Can't open file for reading");
       return 1; 
    }

    char line[64];
    
    // fgets takes string, size, stream
    while (fgets(line, sizeof(line), file))
    {
        tokenise_line(line);
    }
    


    fclose(file);
    return 0;
}