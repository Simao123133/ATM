#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_group(struct Group **Group, char* Name, char* Secret);
void delete_group(struct Group **Group, char* group);
char* check_secret(struct Group *Group, char* group_id, char* secret);
char* find_secret(struct Group *Group, char* group_id);




