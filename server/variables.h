#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define IP_ADDR "172.18.26.3"
#define SOCKNAME "/tmp/client"
#define SOCKNAME_CALLBACK "/tmp/client_callback"


#define ESTABLISH_CONNECTION_MASK '0'
#define PUT_VALUE_MASK '1'
#define GET_VALUE_MASK '2'
#define DELETE_VALUE_MASK '3'
#define CLOSE_CONNECTION_MASK '4'
#define REGISTER_CALLBACK_MASK '5'

#define AUTHENTICATION_MASK '0'
#define CREATE_GROUP_MASK '1'
#define DELETE_GROUP_MASK '2'
#define REQUEST_SECRET_MASK '3'

#define SUCCESS '1'
#define FAILURE '0'
#define WRONG_KEY '0'
#define ALREADY_REGISTERED '2'

// communication with the client
struct sockaddr_un server_addr;
struct sockaddr_un server_addr_callback;
int server_sock, server_sock_callback;

 // Communication with the authentication server
struct sockaddr_in server_addr_net;
struct sockaddr_in local_addr;
int sock_fd_in;

// linked lists

struct Key {
	char* key_name;
    char* value;
    struct Sockets *firstsocket;
	struct Key *nextKey;
};

struct Group {
	char*  Name;
    int    Num_pairs;
    struct Key *firstkey;
	struct Group *nextGroup;
};

struct App{
    pid_t PID;
    char connection_establishing_time[50];
    char connection_close_time[50];
    struct App *nextApp;
};

struct Sockets{
    int socket_id;
    struct Sockets *nextSocket;
};

struct Group* group_data;
struct App* App_data;
