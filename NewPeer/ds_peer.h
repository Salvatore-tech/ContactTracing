//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_DS_PEER_H
#define NEWPEER_DS_PEER_H

#define NODE_SIZE 88

static const unsigned int generatingIdPeriod = 60;

_Noreturn void *addNode();

void deleteNodeList();

void writeNeighbourID(char *neigh_id);

int searchNeighbour(char *target_id, const struct sockaddr_in *previousSender, struct sockaddr_in *positiveSender,
                    int *toSkip);

void printList();

int printMyId();

#endif //NEWPEER_DS_PEER_H
