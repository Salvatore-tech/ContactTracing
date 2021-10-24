//
// Created by saso on 03/09/2021.
//
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "common.h"
#include "peer.h"
#include "ds_peer.h"

int generateMenu() {
    int choice = 0;

    printf("\n1) Contact a peer");
    printf("\n2) View contacts");
    printf("\n3) Show generated id");
    printf("\n0) Terminate\n");
    scanf("%d", &choice);
    return (choice);
}

_Noreturn void *addNodeToList() {
    while (1) {
        node_t *new_node = malloc(NODE_SIZE);

        if (new_node) {
            generate_id(ID_LEN, new_node->id);
            new_node->no_cont = 0;
            new_node->neigh_ids = malloc(0); // Needs to be !NULL, because a realloc will be performed on it
            new_node->next = NULL;

            if (!head) {
                head = calloc(1, sizeof(node_t));
                tail = calloc(1, sizeof(node_t));
                head = tail = new_node;
                tail->next = NULL;
            } else {
                tail->next = new_node;
                tail = new_node;
            }
        }

        sleep(generatingIdPeriod);
    }
}

void writeNeighbourID(char *neigh_id) {
    tail->no_cont++;
    tail->neigh_ids = realloc(tail->neigh_ids, tail->no_cont * ID_LEN);
    strcat(tail->neigh_ids, neigh_id);
}

// Check if there is a match between two if. If so, set toSkip = 1 and it avoids to check all the remaining id
int searchNeighbour(char *target_id, struct sockaddr_in *previousSender, struct sockaddr_in *positiveSender,
                    int *toSkip) {

    if (memcmp(previousSender, positiveSender, sizeof(struct sockaddr_in)) == 0) {
        *toSkip = 1;
        return 0;
    }

    for (node_t *iter = head; iter != NULL && *toSkip == 0; iter = iter->next) {
        char *s = strdup(
                strtok(iter->id, &delimiter)); // Do a copy, because strtok() removes delimiter permanently on the variable

        if ((memcmp(s, target_id, sizeof(*target_id)) == 0)) {
            memcpy(previousSender, positiveSender, sizeof(struct sockaddr_in));
            return 1;
        }
    }
    return 0;
}

void printPeersContacted() {
    int k = 0;
    int choice;
    char *s_copy;
    printf("\nId generated:\n");

    int j = printGeneratedId();

    do {
        printf("\nInsert an index\n");
        scanf("%d", &choice);
    } while (choice < 0 || choice >= j);

    node_t *iter = head;
    for (int i = 0; i < choice; i++)
        iter = iter->next;

    if (iter->neigh_ids) { // Iterates on the neighbours id
        s_copy = strdup(iter->neigh_ids);

        printf("\n");
        for (char *p = strtok(s_copy, "_"); p != NULL; p = strtok(NULL, "_"), k++) {
            printf("[N%d]: %s\n", k, p);
        }

        free(s_copy);

    }
    if (k == 0) {
        puts("You have not been in touch with another peer yet\n");
    }
}

int printGeneratedId() {
    int i = 0;
    for (node_t *tmp = head; tmp != NULL; tmp = tmp->next, i++)
        printf("[Id %d]: %.*s\n", i, ID_LEN - 2, tmp->id); //TODO: edited
    return i;
}

void deleteNodeList() {
    node_t *current = head;
    node_t *next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}
