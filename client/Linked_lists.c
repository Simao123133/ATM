#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "variables.h"

struct Key *find_key(char * key, struct Key *Key){
    while(Key != NULL){
        if(strcmp((Key)->key_name, key)==0)
            return Key;
        Key = Key->nextKey; 
    }

    return NULL;
}

int add_key(char * key, struct Key **Key, void (*callback_function)(char *)){
    
    struct Key *newkey =  (struct Key*) malloc(sizeof(struct Key));
    if(newkey == NULL){
        printf("Malloc error\n");
        return -3;
    }
    
    newkey->key_name = key;
    newkey->callback_function = callback_function;
    newkey->nextKey = *Key;
    *Key = newkey;

    return 1;

}

void clean_all(struct Key* Key){

    struct Key* Key_aux = Key;

    while(Key != NULL){
        Key_aux = Key;
        Key = Key->nextKey;
        free(Key_aux);
    }
        
    return;

}