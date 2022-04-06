#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

char* concatenate_message(char MASK, char* string1, char* string2);
char* read_message(int local_socket);

