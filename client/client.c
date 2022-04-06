#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>

#include "KVS-lib.h"
#include "variables.h"
#include "messages.h"
#include "Linked_lists.h"

void f(char * key);

void* register_callback_thread(void* arg){

    char* key;
    struct Key* Key_aux;
    do{
        key = read_message_callback(sock_callback); 
        Key_aux = find_key(key, Key);
        Key_aux->callback_function(key);
        free(key);
    }while(1);

    pthread_exit;
    
    return NULL;

}

int main(){
      
    char c;
    int connection_established = 0, cond;
    char group_id[MAX_SIZE], secret[MAX_SIZE], key[MAX_SIZE], rcv_value[MAX_SIZE];
    char* value;
    Key = NULL;

    sock = socket(AF_UNIX, SOCK_STREAM, 0); 
    sock_callback = socket(AF_UNIX, SOCK_STREAM, 0);

    if(sock == -1 || sock_callback == -1){
        perror("socket creation");
        exit(-1);
    }

    memset(&server_addr,0,sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKNAME);
    
    memset(&server_addr_callback, 0, sizeof(struct sockaddr_un));
    server_addr_callback.sun_family = AF_UNIX;
    strcpy(server_addr_callback.sun_path, SOCKNAME_CALLBACK);

    while(1){

        printf("Waiting for console commands.\n");
        printf("0 - Establish connection\n");
        printf("1 - Put value\n");
        printf("2 - Get value\n");
        printf("3 - Delete value\n");
        printf("4 - Close connection\n");
        printf("5 - Register callback\n");
        printf("6 - exit\n");
        
        scanf(" %c",&c);

        switch (c) {
            case ESTABLISH_CONNECTION_MASK:
                if (connection_established){
                    printf("Already connected\n");
                    break;
                }
                printf("Insert group id: \n");
                scanf("%s", group_id);
                printf("Insert secret: \n");
                scanf("%s",secret);
                cond = establish_connection(group_id, secret);
                if(cond == 0){
                    connection_established = 1;
                    if (connect(sock_callback, (struct sockaddr *) &server_addr_callback, sizeof(struct sockaddr_un))==-1){
                        printf("Error connecting to server callback socket\n");
                        clean_all(Key);
                        exit(-2);
                    }
                    pthread_t t_id;
                    if(pthread_create(&t_id, NULL, register_callback_thread, NULL) != 0){
                        printf("Error creating callback thread\n");
                        clean_all(Key);
                        exit(-4);
                    }
                }else if(cond == -2){
                    exit(-2);
                }
                break;
            
            case PUT_VALUE_MASK:
                if (!connection_established){
                    printf("Establish connection first\n");
                    break;
                }
                printf("Insert key: \n");
                scanf("%s", key);
                printf("Insert value: \n");
                scanf("%s", rcv_value);
                if (put_value(key, rcv_value) == -1){
                    clean_all(Key);
                    exit(-3);
                }
                break;
            
            case GET_VALUE_MASK:
                if (!connection_established){
                    printf("Establish connection first\n");
                    break;
                }
                printf("Insert key: \n");
                scanf("%s", key);
                if(get_value(key, &value) == -3){
                    clean_all(Key);
                    free(value);
                    exit(-3);
                }
                free(value);
                break;
            
            case DELETE_VALUE_MASK:
                if (!connection_established){
                    printf("Establish connection first\n");
                    break;
                }
                printf("Insert key: \n");
                scanf("%s", key);
                if(delete_value(key) == -3){
                    clean_all(Key);
                    exit(-3);
                }
                break;

            case CLOSE_CONNECTION_MASK:
                if (!connection_established){
                    printf("Establish connection first\n");
                    break;
                }
                if(close_connection() == -2){
                    printf("Error closing sockets\n");
                    clean_all(Key);
                    exit(-2);
                }
                break;

            case REGISTER_CALLBACK_MASK:
                if (!connection_established){
                    printf("Establish connection first\n");
                    break;
                }
                printf("Insert key to be monitored: \n");
                scanf("%s", key);
                if(register_callback(key, f) == -3){
                    clean_all(Key);
                    exit(-3);
                }
                break;

            case EXIT:
                clean_all(Key);
                return 1;
    
            default:
                printf("Wrong Mask\n");
        }
    }
    
    return 0;
}

void f(char * key){
    printf("Key %s was changed\n", key);
}

