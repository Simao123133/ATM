#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

char* concatenate_message(char MASK, char* string1, char* string2){

    char *result_str = NULL;
    int finalSize = 0;

    finalSize = 1 + (int)strlen(string1); 
    result_str = (char *)malloc((finalSize + 1) * sizeof(char));
    if(result_str == NULL){
            printf("MALLOC error\n");
            return NULL;
    }
    result_str[0] = MASK;
    result_str[1]= '\0'; 
    strcat(result_str, string1);

    if(string2 != NULL){
        result_str[(int)strlen(string2)+1]= '\n';
        finalSize = finalSize + 1 + (int)strlen(string2);
        result_str = realloc(result_str,(finalSize + 1)*sizeof(char));
        if(result_str == NULL){
            printf("Realloc error\n");
            return NULL;
        }
        strcat(result_str, string2);
        result_str[(int)strlen(string2)+1]= '\0';
    }
    
    return result_str;

}

char* read_message(int local_socket, char *success){
    
    char *line = malloc(sizeof(char));
    char c;
    int i=0;

    read(local_socket, success, 1);

    if(success){
        do{
            read(local_socket, &c, 1);
            line[i] = c;
            i++;
            line = realloc(line, i+1);
            if(line == NULL){
                printf("Realloc error\n");
                return NULL;
            }
            line[i] = '\0';
        }while(c != '\0');
        return line; 
    }

    free(line);
    return NULL;
   
}

char* read_message_callback(int local_socket){
    
    char *line = malloc(sizeof(char));
    char c;
    int i=0;

    do{
        read(local_socket, &c, 1);
        line[i] = c;
        i++;
        line = realloc(line, i+1);
        line[i] = '\0';
    }while(c != '\0');

    return line;    
}