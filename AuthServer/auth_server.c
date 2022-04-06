#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include "variables.h"
#include "linked_lists.h"

void* local_server_thread(void* arg){

    struct local_server_data *data = arg;
    char* group_id;
    char* secret;
    int i=0;

    group_id = data->msg+1;
    if(data->msg[0] == AUTHENTICATION_MASK || data->msg[0] == CREATE_GROUP_MASK){
        do{
            i++;
        }while(data->msg[i-1] != '\0');
        secret = data->msg+i;
    }

    switch (data->msg[0]) {
        case AUTHENTICATION_MASK:
            sendto(sock_fd, check_secret(group_data, group_id, secret), strlen("1"), 0, (struct sockaddr *) &(data->other_addr), data->size_other_addr);
            break;
        case CREATE_GROUP_MASK:
            create_group(&group_data, group_id, secret);
            break;
        case DELETE_GROUP_MASK:
            delete_group(&group_data, group_id);
            break;
        case REQUEST_SECRET_MASK:
            sendto(sock_fd, find_secret(group_data, group_id), strlen(find_secret(group_data, group_id))+1, 0, (struct sockaddr *) &(data->other_addr), data->size_other_addr);
            break;
        default:
            printf("Wrong Mask\n");
    }

    free(data);

    return 0;
}

int main(){

    char buff1[1000];
    int nbytes;
    struct local_server_data *data;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1){
        perror("socket: error");
        exit(-1);
    }
    
    memset(&local_addr, 0, sizeof(struct sockaddr_in));

    local_addr.sin_family = AF_INET;
    inet_aton("0.0.0.0", &local_addr.sin_addr);
    local_addr.sin_port = htons(3004);
    
    if(bind(sock_fd, (struct sockaddr *) &local_addr, sizeof(local_addr)) == -1){
        perror("Bind");
        exit(-2);
    }
    
    do{
        size_other_addr = sizeof(struct sockaddr_in);
        nbytes = recvfrom(sock_fd, buff1, 1000, 0, (struct sockaddr *) &other_addr, &size_other_addr);
        data = malloc(sizeof(struct local_server_data));
        data->other_addr = other_addr;
        data->size_other_addr = size_other_addr;
        data->msg = buff1;
        pthread_t t_id;
        pthread_create(&t_id, NULL, local_server_thread, data);
    }while(nbytes > 0);

    return 0;
}

