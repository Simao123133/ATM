#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_group_existence(struct Group *Group, char* group);
void list_create_group(struct Group **Group, char* Name);
void deleteKeys(struct Key *Key);
void list_delete_group(struct Group **Group, char* group);
int get_num_pairs(struct Group *Group, char* group);
void find_key_and_key_prev(struct Group *Group, char* group, char * key, struct Key **Key, struct Key **Key_prev);
void find_key(struct Group *Group, char* group, char * key, struct Key **Key);
void put_value(struct Group *Group, char* group, char * key, char * value);
char get_value(struct Group *Group, char* group, char * key, char ** value);
int delete_value(struct Group *Group, char* group, char * key);
char*get_time();
void list_show_application_status(struct App* App);
void create_new_app(struct App** App);
void app_connection_close_time(struct App* App, pid_t PID);
int find_socket(struct Key *Key, char* socket);
void add_socket(struct Key *Key, int socket_id);
void clean_all(struct Group* Group, struct App* App);






