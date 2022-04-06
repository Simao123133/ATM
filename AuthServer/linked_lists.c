#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "variables.h"

void create_group(struct Group **Group, char* Name, char* Secret){

    struct Group *newgroup = NULL;

    newgroup = (struct Group*) malloc(sizeof(struct Group));
	
    if(newgroup != NULL){
		newgroup->nextGroup = *Group;
        newgroup->Name = malloc((strlen(Name)+1) * sizeof(char));  
        strcpy(newgroup->Name, Name);           
        newgroup->Secret = malloc((strlen(Secret)+1) * sizeof(char));  
        strcpy(newgroup->Secret, Secret);
        printf("Group created with id: %s and secret %s\n",newgroup->Name, newgroup->Secret); 
	}
	else
		printf("Allocation Failure!! \n");
  
    (*Group)  = newgroup;

    return;

} 

void delete_group(struct Group **Group, char* group){
    
    struct Group *Group_aux = *Group;
    struct Group *Group_aux_prev = NULL;

    while(Group_aux != NULL){
        if(strcmp(Group_aux->Name,group)==0){
            if(Group_aux_prev != NULL)
                Group_aux_prev->nextGroup=Group_aux->nextGroup;
            else
                *Group = (*Group)->nextGroup;
            free(Group_aux->Name);
            free(Group_aux->Secret);
            free(Group_aux);
            printf("Group deleted with id: %s\n",group); 
            return;
        }
        Group_aux_prev = Group_aux;
        Group_aux = Group_aux->nextGroup;
    }
    printf("Group not found\n"); 
    return;

} 


char* check_secret(struct Group *Group, char* group_id, char* secret){

    while(Group != NULL){
        if(strcmp(Group->Name, group_id) == 0){
            if(strcmp(Group->Secret, secret) == 0)
                return "1";
            else 
                return "0";
        }
        Group = Group->nextGroup;
    }
    return "0";
}

char* find_secret(struct Group *Group, char* group_id){

    while(Group != NULL)
         if(strcmp(Group->Name, group_id) == 0)
            return Group->Secret;
}





