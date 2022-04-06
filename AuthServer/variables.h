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

#define AUTHENTICATION_MASK '0'
#define CREATE_GROUP_MASK '1'
#define DELETE_GROUP_MASK '2'
#define REQUEST_SECRET_MASK '3'

struct sockaddr_in local_addr;
struct sockaddr_in other_addr;
socklen_t size_other_addr;

int sock_fd;
int server_sock;

struct local_server_data {
    struct sockaddr_in other_addr;
    char* msg;
    socklen_t size_other_addr;
};

struct Group {
	char*  Name;
    char*  Secret;
    struct Key *firstkey;
	struct Group *nextGroup;
};

struct Group *group_data;
