//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_DS_PEER_H
#define NEWPEER_DS_PEER_H

#define NODE_SIZE 88

static const unsigned int generatingIdPeriod = 80;

int generateMenu();

_Noreturn void *addNodeToList();

void writeNeighbourID(char *neigh_id);

int searchNeighbour(char *target_id, struct sockaddr_in *previousSender, struct sockaddr_in *positiveSender,
                    int *toSkip);

void printPeersContacted();

int printGeneratedId();

void deleteNodeList();

#endif //NEWPEER_DS_PEER_H
