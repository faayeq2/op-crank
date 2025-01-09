#include <stdio.h>

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

    char line_buffer[256];
    
}