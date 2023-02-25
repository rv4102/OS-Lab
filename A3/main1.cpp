#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>


typedef struct DNode DNode;
typedef struct DList DList;

#define MAX_DNODE 32            // Max. domain string length
#define MAX_DLEN 64             // Max. number of list nodes

#define DNULL (MAX_DLEN + 1)    // NULL value

struct DNode {
    int value;
    size_t parent;
    size_t next;
};

struct DList {
    DNode pool[MAX_DNODE];      // fixed-size space for nodes
    size_t npool;               // used space in pool
    size_t pfree;               // pointer to re-use freed nodes
    size_t head;                // global list head
};

DList *dlist;

DNode *dnode_alloc(void) {
    if (dlist->pfree != DNULL) {
        DNode *node = dlist->pool + dlist->pfree;

        dlist->pfree = dlist->pool[dlist->pfree].next;
        return node;
    }
    else {
        if (dlist->npool < MAX_DNODE) 
            return &dlist->pool[dlist->npool++];
    }

    return NULL;
}

void dnode_free(DNode *node) {
    if (node) {
        node->next = dlist->pfree;
        dlist->pfree = node - dlist->pool;
    }
}

DNode *dnode(size_t index) {
    return (index == DNULL) ? NULL : dlist->pool + index;
}

DNode *dnode_next(const DNode *node) {
    return dnode(node->next);
}

DNode *dnode_push(size_t *head, const char *str) {
    DNode *node = dnode_alloc();

    if (node) {
        strncpy(node->domain, str, sizeof(node->domain));
        node->domain = 
        node->next = *head;
        *head = node - dlist->pool;
    }

    return node;
}

void dnode_pop(size_t *head)
{
    if (*head != DNULL) {
        size_t next = dlist->pool[*head].next;

        dnode_free(&dlist->pool[*head]);
        *head = next;
    }
}


int main(int argc, char* argv[])
{
    int shmid;

    shmid = shmget(IPC_PRIVATE, sizeof(DList), IPC_CREAT | 0660);
    if (shmid < 0) exit(1);

    dlist = shmat(shmid, NULL, 0);
    if (dlist == (void *) (-1)) exit(1);

    dlist->head = DNULL;
    dlist->pfree = DNULL;
    dlist->npool = 0;

    dnode_push(&dlist->head, "Alpha");
    dnode_push(&dlist->head, "Bravo");
    dnode_push(&dlist->head, "Charlie");
    dnode_push(&dlist->head, "Delta");
    dnode_push(&dlist->head, "Echo");

    while (dlist->head != DNULL) {
        puts(dnode(dlist->head)->domain);
        dnode_pop(&dlist->head);
    }

    shmdt(dlist);

    return 0;
}