#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>

#define SOCKNAME "/tmp/client"
#define SOCKNAME_CALLBACK "/tmp/client_callback"

#define ESTABLISH_CONNECTION_MASK '0'
#define PUT_VALUE_MASK '1'
#define GET_VALUE_MASK '2'
#define DELETE_VALUE_MASK '3'
#define CLOSE_CONNECTION_MASK '4'
#define REGISTER_CALLBACK_MASK '5'
#define EXIT '6'

#define FAILED '0'

#define MAX_SIZE 1000

#define WRONG_KEY '0'
#define ALREADY_REGISTERED '2'


int sock, sock_callback;
struct sockaddr_un server_addr;
struct sockaddr_un server_addr_callback;

struct Key {
	char* key_name;
    void (*callback_function)(char *);
	struct Key *nextKey;
};

struct Key* Key;