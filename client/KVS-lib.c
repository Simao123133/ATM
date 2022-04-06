#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "variables.h"
#include "messages.h"
#include "Linked_lists.h"

int establish_connection(char * group_id, char * secret){

    char *msg, success;

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un))==-1){
        printf("Error connecting to server socket\n");
        return -2;
    }
    
    msg = concatenate_message(ESTABLISH_CONNECTION_MASK, group_id, secret);

    write(sock, msg, 1+strlen(group_id)+1+strlen(secret)+1);

    free(msg);

    read(sock, &success, 1);
    if(success == FAILED){
        printf("Authentication unsuccessful\n");
        return -1;
    }
    else{
        printf("Authentication successful\n");
        return 0;
    }

}

int put_value(char* key, char* value){

    char *msg = NULL, success;

    msg = concatenate_message(PUT_VALUE_MASK, key, value);
    if(msg == NULL)
        return -3;

    write(sock,msg, 1+strlen(key)+1+strlen(value)+1);

    free(msg);
    
    return 1;

}

int get_value(char* key, char** value){

    char *msg, success;

 
    msg = concatenate_message(GET_VALUE_MASK, key, NULL);
    if(msg == NULL)
        return -3;

    write(sock, msg, 1 + (int)strlen(key) + 1); 

    *value = read_message(sock, &success);
    if(*value == NULL)
        return -3;

    free(msg);

    if(success == FAILED){
        printf("Key does not exist\n");
        return -1;
    }
    else   
        printf("The value is: %s\n", *value);

    return 1;

}

int delete_value(char* key){
    
    char *msg, *value, success;
 
    msg = concatenate_message(DELETE_VALUE_MASK, key, NULL);
    if(msg == NULL)
        return -3;

    write(sock, msg, 1+(int)strlen(key)+1); 
    free(msg);

    read(sock, &success, 1);

    if(success == FAILED){
        printf("Key does not exist");
        return -1;
    }
    else
        printf("Key-value deleted\n");
    
    return 1;

}

int register_callback(char* key, void (*callback_function)(char *)){
    
    char* msg, result;

    msg = concatenate_message(REGISTER_CALLBACK_MASK, key, NULL);
    if(msg == NULL)
        return -3;

    write(sock, msg, (int)strlen(msg)+1);

    read(sock, &result, 1);

    if(result == WRONG_KEY){
        printf("Wrong Key\n");
        return -1;
    }
    else if(result == ALREADY_REGISTERED){
        printf("Already Registered");
        return -1;
    }
    else{
        if(add_key(key, &Key, callback_function) == -3)
            return -3;
    }

    free(msg);

    return 1;

}

int close_connection(){
    
    write(sock,"4", 1+1);
    
    if(close(sock) == -1 || close(sock_callback) == -1)
        return -2;

    return 1;

}

