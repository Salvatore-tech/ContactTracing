//
// Created by saso on 07/08/21.
//

#ifndef P2P_DS_PEER_H
#include "header.h"
#define P2P_DS_PEER_H
#define PEER_NO 10


void *addNode();
void writeNeighbourID(char *neigh_id);
int searchNeighbourInNode(char **a_neighs, char *target_id);
int searchNeighbour(char* target_id);
void printList(node_t *head);
#endif //P2P_DS_PEER_H
