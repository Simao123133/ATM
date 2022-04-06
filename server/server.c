#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "variables.h"
#include "KVS-LocalServer.h" //console functions
#include "Linked_lists.h"
#include "messages.h"


char authenticate(char* group_id, char* secret, int local_socket);

//thread functions

void* client_thread(void* arg){

    int local_socket = (int) arg;
    int callback_socket;
    char *group_id, *secret, *key, *value = NULL, *result = malloc(sizeof(char));
    char *line, *msg;
    struct Key* Key = NULL;
    pid_t PID = getpid();

    while(1){

        line = read_message(local_socket);
        switch (line[0]) {
            case ESTABLISH_CONNECTION_MASK:
                group_id = line+1;
                if(!check_group_existence(group_data, group_id)){
                    result[0] = FAILURE;
                    write(local_socket, result, 1);
                    free(line);
                    pthread_exit;
                    return NULL;

                }
                else{
                    secret = read_message(local_socket);
                    result[0] = authenticate(group_id, secret, local_socket);
                    if(result[0] == FAILURE){
                        free(line);
                        pthread_exit;
                        return NULL;
                    }
                    write(local_socket, result, 1);
                    callback_socket = accept(server_sock_callback, NULL, NULL); 
                    create_new_app(&App_data);
                    free(secret);
                }
                break;
            
            case PUT_VALUE_MASK:
                key = line+1;
                value = read_message(local_socket);
                put_value(group_data, group_id, key, value);
                free(line);
                break;
            
            case GET_VALUE_MASK:
                key = line+1;
                result[0] = get_value(group_data, group_id, key, &value);
                msg = concatenate_message(result[0], value, NULL);
                write(local_socket, msg, (int)strlen(msg) + 1);
                free(value);
                free(msg);
                free(line);
                break;

            case DELETE_VALUE_MASK:
                key = line+1;
                result[0] = delete_value(group_data, group_id, key);
                write(local_socket, result, 1);
                free(line);
                break;

            case CLOSE_CONNECTION_MASK:
                app_connection_close_time(App_data, PID);
                close(local_socket);
                free(group_id-1);
                free(result);
                free(line);
                pthread_exit;
                return NULL;

            case REGISTER_CALLBACK_MASK:
                key = line+1;
                find_key(group_data, group_id, key, &Key);
                if(Key == NULL)
                    result[0] = WRONG_KEY;
                else if(find_socket(Key, callback_socket))
                    result[0] = ALREADY_REGISTERED;          
                else{
                    add_socket(Key, callback_socket);
                    result[0] = SUCCESS;   
                }
                write(local_socket, result, 1); 
                free(line);
                break;

            default:
                printf("Wrong Mask\n");
        }
    }
    
}

void* console_thread(void* arg){
    int d;
    char group_id[1000];
    
    while(1){

        printf("Waiting for console commands.\n");
        printf("1 - Create Group\n");
        printf("2 - Delete Group\n");
        printf("3 - Show Group Info\n");
        printf("4 - Show Application Status\n");
        printf("5 - Exit\n");
        
        scanf("%d",&d);

        switch (d){
            case 1:
                printf("Group_id:\n");
                scanf("%s",group_id);
                create_group(group_id);
                break;
            case 2:
                printf("Group_id:\n");
                scanf("%s",group_id);
                delete_group(group_id);
                break;
            case 3:
                printf("Group_id:\n");
                scanf("%s",group_id);
                show_group_info(group_id);
                break;
            case 4:
                show_application_status();
                break; 
            case 5:
                clean_all(group_data, App_data);
                exit(0);
                break;
        }
    }
    return (void*) NULL;
}

int main(){

    // communication with the client

        // communication socket
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKNAME);

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if(server_sock == -1){
        perror("socket creation");
        exit(-1);
    }

    unlink(SOCKNAME);
    if(bind(server_sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)) == -1){
        perror("Bind");
        exit(-2);
    }
    
    if (listen(server_sock, 5) == -1){
        perror("Listen");
        exit(-3);
    }

        //callback socket

    memset(&server_addr_callback, 0, sizeof(struct sockaddr_un));
    server_addr_callback.sun_family = AF_UNIX;
    strcpy(server_addr_callback.sun_path, SOCKNAME_CALLBACK);

    server_sock_callback = socket(AF_UNIX, SOCK_STREAM, 0);

    if(server_sock_callback == -1){
        perror("socket creation");
        exit(-1);
    }

    unlink(SOCKNAME_CALLBACK);
    if(bind(server_sock_callback, (struct sockaddr *) &server_addr_callback, sizeof(struct sockaddr_un)) == -1){
        perror("Bind");
        exit(-2);
    }
    
    if (listen(server_sock_callback, 5) == -1){
        perror("Listen");
        exit(-3);
    }
 
    // Communication with the authentication server
    
    sock_fd_in = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock_fd_in == -1){
        perror("socket creation");
        exit(-4);
    }
    memset(&local_addr, 0, sizeof(struct sockaddr_in));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(3001);

    if(bind(sock_fd_in, (struct sockaddr *)&local_addr, sizeof(local_addr)) == -1){
        perror("bind");
        exit(-5);
    }
    memset(&server_addr_net, 0, sizeof(struct sockaddr_in));
    server_addr_net.sin_family = AF_INET;
    inet_aton(IP_ADDR, &server_addr_net.sin_addr);
    server_addr_net.sin_port = htons(3004);

    //console thread

    pthread_t t_id;
    pthread_create(&t_id, NULL, console_thread, NULL);
    
    //accept new clients

    while(1){
        int local_socket = accept(server_sock, NULL, NULL);
        pthread_t t_id;
        pthread_create(&t_id, NULL, client_thread, local_socket);
    }
    
    //close sockets

    close(sock_fd_in);
    close(server_sock);
    close(server_sock_callback);

    return 0;
}

char authenticate(char* group_id, char* secret, int local_socket){

    char auth_result[1]; 
    char *msg = NULL;

    msg = concatenate_message(AUTHENTICATION_MASK, group_id, secret);
    
    sendto(sock_fd_in, msg, strlen(msg)+1, 0, (struct sockaddr *) &server_addr_net, sizeof(struct sockaddr_in));
    recvfrom(sock_fd_in, auth_result, sizeof(auth_result), 0, NULL, NULL);

    free(msg);

    return auth_result[0];

}


