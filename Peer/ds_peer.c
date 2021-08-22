//
// Created by saso on 07/08/21.
//

#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "ds_peer.h"
#include "peer.h"

void *addNode(){
    node_t *new_node = calloc(1, sizeof(node_t));
    new_node->id = generate_id(64);
    new_node->next = NULL;
    if (!head)
    {
        head = calloc(1, sizeof (node_t));
        tail = calloc(1, sizeof (node_t));
        head = tail = new_node;
        tail->next = NULL;
    }
    else
    {
        tail->next = new_node;
        tail = new_node;
    }
    //    sleep(2);
    //    addNode();
}

void writeNeighbourID(char *neigh_id)
{
    if(!tail->neigh_ids)
        tail->neigh_ids= malloc(sizeof (char*));
    strcat(tail->neigh_ids, neigh_id);
}



int searchNeighbour(char* target_id){
    for(node_t* iter=head; iter != tail; iter=iter->next)
        // TODO: controllare se il confronto tra il mio id e il target id è corretto
        if (memcmp(strtok(iter->id, ":"), target_id, sizeof(*target_id) == 0))
            return 1;
     return 0;
}

void printList(node_t *head)
{
    while (head != NULL){
        printf("%s", head->id);
        head = head->next;
    }
}