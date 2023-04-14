#ifndef GOODMALLOC_H
#define GOODMALLOC_H

#include <stdlib.h>
#include <cassert>
#define dbg(x) cout << #x << " = " << x << endl;
// struct to represent element
typedef struct element{
    int val;
    int prev; // offset value
    int next; // offset value
}element;       // 12 bytes

// struct to represent a list
typedef struct list{
    int head; // offset
    int tail; // offset
    int size;
}list;

void *createMem(size_t size);
void *createList(size_t size, const char *name);
int assignVal(const char *name, int index, int val);
int getVal(list *l, int index);
void freeElem(char *name);
void freeElem();

void initScope();
void endScope();
void printList(list *l);
char *getAddr(int page_table_index);

#endif
