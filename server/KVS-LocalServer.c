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
#include "Linked_lists.h"

int create_group(char* group_id){

    if(check_group_existence(group_data, group_id)){
            printf("Group already exists");
            return -1;
    }

    char* secret;
    char *msg = NULL;
    int finalSize = 0; 

    list_create_group(&group_data, group_id);

    secret = group_id;

    finalSize = 1 + (int)strlen(group_id) + 1 + (int)strlen(secret);
    msg = (char *)malloc((finalSize + 1) * sizeof(char));
    msg[0] = CREATE_GROUP_MASK;
    msg[1]= '\0'; 
    strcat(msg, group_id);
    msg[(int)strlen(group_id)+1]= '\n';
    strcat(msg, secret);
    msg[(int)strlen(group_id)+1]= '\0';

    sendto(sock_fd_in, msg, 1 + strlen(group_id)+ 1 + strlen(secret) + 1, 0, (struct sockaddr *) &server_addr_net, sizeof(struct sockaddr_in));

    free(msg);

    return 0;

}

int delete_group(char* group_id){

    if(!check_group_existence(group_data, group_id)){
        printf("Group does not exist");
        return -1;
    }


    int finalSize = 1 + (int)strlen(group_id);
    char* msg = (char *)malloc((finalSize + 1) * sizeof(char));

    list_delete_group(&group_data, group_id);

    msg[0] = DELETE_GROUP_MASK;
    msg[1]= '\0'; 
    strcat(msg, group_id);

    sendto(sock_fd_in, msg, strlen(msg)+1, 0, (struct sockaddr *) &server_addr_net, sizeof(struct sockaddr_in));

    free(msg);

    return 0;

}

int show_group_info(char* group_id){

    if(!check_group_existence(group_data, group_id)){
        printf("Group does not exist\n");
        return -1;
    }

    int finalSize = 1 + (int)strlen(group_id);
    char* msg = (char *)malloc((finalSize + 1) * sizeof(char));
    char buff1[1000];

    int num_pairs = get_num_pairs(group_data, group_id);

    msg[0] = REQUEST_SECRET_MASK;
    msg[1]= '\0'; 
    strcat(msg, group_id);

    sendto(sock_fd_in, msg, strlen(msg)+1, 0, (struct sockaddr *) &server_addr_net, sizeof(struct sockaddr_in));
    recvfrom(sock_fd_in, buff1, 1000, 0, NULL, NULL);

    printf("Secret: %s\n", buff1);
    printf("Number of key-value pairs: %d\n", num_pairs);

    free(msg);

    return 0;

}

int show_application_status(){

    list_show_application_status(App_data);
    
    return 0;
}