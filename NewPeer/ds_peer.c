//
// Created by saso on 03/09/2021.
//
#include <string.h>
#include <malloc.h>
#include "peer.h"
#include "ds_peer.h"

int neigh_count = 0;

void *addNode() {
    node_t *new_node = calloc(1, sizeof(node_t));
    new_node->id = malloc(sizeof(char) * (ID_LEN + 1));
    new_node->neigh_ids = calloc(1, sizeof (char*));
    new_node->next = NULL;

    generate_id(ID_LEN, new_node->id);

    if (!head) {
        head = calloc(1, sizeof(node_t));
        tail = calloc(1, sizeof(node_t));
        head = tail = new_node;
        tail->next = NULL;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    //    sleep(2);
    //    addNode();
}

void writeNeighbourID(char *neigh_id) {
    strcat(tail->neigh_ids, neigh_id);
}


int searchNeighbour(char *target_id) {
    for (node_t *iter = head; iter != NULL; iter = iter->next) {
        char *s = strtok(iter->id, "_");
        while (s != NULL) {
            strtok(s, "_");
            if ((memcmp(s, target_id, ID_LEN) == 0))
                return 1;
            s = strtok(0, "_");
        }

    }
    // TODO: controllare se il confronto tra il mio id e il target id è corretto

    return 0;
}

void printList() {
    int idx;
    char* s_copy;
    printf("Id generati:\n");
    printMyId(head);

    printf("Inserisci l'indice relativo all'id\n");
    scanf("%d", &idx);


    node_t *iter = head;
    for (int i = 0; i < idx; i++)
        iter = iter->next;

    if (iter->neigh_ids) {
        s_copy = malloc(sizeof (iter->neigh_ids));
        strcpy(s_copy, iter->neigh_ids);

        int i = 0;

        for (char *p = strtok(s_copy, "_"); p != NULL; p = strtok(NULL, "_"), i++) {
            puts(p);
        }
        free(s_copy);

    } else {
        puts("You have not been in touch with other peer\n");
    }


}

void printMyId(node_t *head) {
    int i = 0;
    for (node_t *tmp = head; tmp != NULL; tmp = tmp->next, i++)
        printf("[Id %d]: %.*s\n", i, ID_LEN-2, tmp->id);
}