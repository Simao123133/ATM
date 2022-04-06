#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "variables.h"
#include "messages.h"

int check_group_existence(struct Group *Group, char* group){

    while(Group != NULL){
        if(strcmp(Group->Name, group) == 0)
            return 1;
        Group=Group->nextGroup;
    }
    return 0;
}

void list_create_group(struct Group **Group, char* Name){

    struct Group *newgroup = NULL;

    newgroup = (struct Group*) malloc(sizeof(struct Group));
	
    if(newgroup != NULL){
		newgroup->nextGroup = *Group;
        newgroup->Num_pairs = 0;
        newgroup->firstkey = NULL;
        newgroup->Name = malloc((strlen(Name)+1) * sizeof(char));  
        strcpy(newgroup->Name, Name);           
	}
	else
		printf("Allocation Failure!! \n");
  
    (*Group)  = newgroup;

    return;

} 

void deleteKeys(struct Key *Key) {

    struct Key * Key_aux = NULL;

    while(Key != NULL){
        Key_aux = Key;
        deleteSockets(Key->firstsocket);
        Key = Key->nextKey;
        free(Key_aux->key_name);
        free(Key_aux->value);
        free(Key_aux);
    }

    return;

} 

void list_delete_group(struct Group **Group, char* group){
    
    struct Group *Group_aux = *Group;
    struct Group *Group_aux_prev = NULL;

    while(Group_aux != NULL){
        if(strcmp(Group_aux->Name,group)==0){
            deleteKeys(Group_aux->firstkey);
            if(Group_aux_prev != NULL)
                Group_aux_prev->nextGroup=Group_aux->nextGroup;
            else
                *Group = (*Group)->nextGroup;
            free(Group_aux->Name);
            free(Group_aux);
            return;
        }
        Group_aux_prev = Group_aux;
        Group_aux = Group_aux->nextGroup;
    }

    return;

} 

int get_num_pairs(struct Group *Group, char* group){
    
    while(Group != NULL){
        if(strcmp(Group->Name,group)==0){
            return Group->Num_pairs;
        }
        Group = Group->nextGroup;
    }
    
    printf("Group not found.\n");

    return -1;

}

void find_key_and_key_prev(struct Group *Group, char* group, char * key, struct Key **Key, struct Key **Key_prev){

    while(Group != NULL){
        if(strcmp(Group->Name, group)==0){
            *Key = Group->firstkey;
            while(*Key != NULL){
                if(strcmp((*Key)->key_name, key)==0){
                    return;
                }
                *Key_prev = *Key;
                *Key = (*Key)->nextKey; 
            }
            return;
        }
        Group = Group->nextGroup;
    }

    return;

}

void find_key(struct Group *Group, char* group, char * key, struct Key **Key){

    while(Group != NULL){
        if(strcmp(Group->Name, group)==0){
            *Key = Group->firstkey;
            while(*Key != NULL){
                if(strcmp((*Key)->key_name, key)==0){
                    return;
                }
                *Key = (*Key)->nextKey; 
            }
            return;
        }
        Group = Group->nextGroup;
    }

    return;

}

void put_value(struct Group *Group, char* group, char * key, char * value){

    struct Key *Key=NULL;
    struct Sockets *Sockets=NULL;
    char* msg;
    find_key(Group,group,key,&Key);

    if(Key == NULL){ 
        Key = (struct Key*) malloc(sizeof(struct Key));
        Key->key_name =  (char*) malloc((strlen(key)+1) * sizeof(char));
        strcpy(Key->key_name, key);
        Key->value =  (char*) malloc((strlen(value)+1) * sizeof(char));
        strcpy(Key->value, value);
        Key->nextKey = Group->firstkey;
        Group->firstkey = Key;
        Group->Num_pairs = Group->Num_pairs+1;
    }
    else{
        if(strcmp(Key->value, value) == 0)
            return;
        else{
            strcpy(Key->value, value);
            Sockets = Key->firstsocket;
            while(Sockets != NULL){
                write(Sockets->socket_id, key, strlen(key)+1);
                Sockets = Sockets->nextSocket;
            }
        }
    }

    return;

}

char get_value(struct Group *Group, char* group, char * key, char ** value){

    struct Key *Key=NULL;

    find_key(Group,group,key,&Key);

    if(Key == NULL)
        return FAILURE;
    else{
        *value =  (char*) malloc((strlen(Key->value)+1) * sizeof(char));
        strcpy(*value, Key->value);
        return SUCCESS;
    }
    
}

char delete_value(struct Group *Group, char* group, char * key){

    struct Key *Key=NULL;
    struct Key *Key_prev=NULL;

    find_key_and_key_prev(Group, group, key, &Key, &Key_prev);

    if(Key == NULL)
        return FAILURE;
    else{
        if(Key_prev != NULL)
            Key_prev->nextKey=Key->nextKey;
        else
            Group->firstkey = Group->firstkey->nextKey;
        Group->Num_pairs--;

        free(Key->key_name);
        free(Key->value);
        free(Key);
        
        return SUCCESS;
    }

    return 0;
        
}

void list_show_application_status( struct App* App){

    int flag = 0;

    while(App != NULL){
        printf("Pid: %d, Connection Establishing Time: %sConnection close time: %s\n", App->PID, App->connection_establishing_time, App->connection_close_time);
        App = App->nextApp;
        flag = 1;
    }
    
    if(flag == 0)
        printf("No apps found\n");
    
    return;

}

char* get_time(){

    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    return asctime(timeinfo);

}

void create_new_app(struct App** App){

    struct App* newapp = (struct App*) malloc(sizeof(struct App));

    newapp->PID = getpid();
    strcpy(newapp->connection_establishing_time, get_time());
    strcpy(newapp->connection_close_time, "0");

    newapp->nextApp = (*App);
    *App = newapp;

    return;

}

void app_connection_close_time(struct App* App, pid_t PID){

    while(App != NULL){
        if(App->PID == PID){
            strcpy(App->connection_close_time, get_time());
            return;
        }
        App = App->nextApp; 
    }

    return;

}

int find_socket(struct Key *Key, int socket){

    struct Sockets *Socket = Key->firstsocket;

    while(Socket != NULL){
        if(Socket->socket_id == socket)
            return 1;
        Socket = Socket->nextSocket; 
    } 

    return 0;

}

void add_socket(struct Key *Key, int socket_id){

    struct Sockets *newsocket = (struct Sockets *)malloc(sizeof(struct Sockets));

    newsocket->socket_id = socket_id;

    newsocket->nextSocket = Key->firstsocket;
    Key->firstsocket = newsocket;

}

void deleteSockets(struct Sockets *Socket) {

    struct Sockets * Socket_aux = NULL;

    while(Socket != NULL){
        Socket_aux = Socket;
        Socket = Socket->nextSocket;
        free(Socket_aux);
    }

    return;

} 

void clean_all(struct Group* Group, struct App* App){

    struct App* App_aux = App;

    while(Group != NULL)
        list_delete_group(&Group, Group->Name);

    while(App != NULL){
        App_aux = App;
        App = App->nextApp;
        free(App_aux);
    }
        
    return;

}



