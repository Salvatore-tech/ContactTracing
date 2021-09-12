//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_DS_PEER_H
#define NEWPEER_DS_PEER_H
#include "header.h"
void *addNode();
void writeNeighbourID(char *neigh_id);
int searchNeighbourInNode(char **a_neighs, char *target_id);
int searchNeighbour(char* target_id);
void printList();
void printMyId(node_t* head);
#endif //NEWPEER_DS_PEER_H
