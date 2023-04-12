#ifndef GOODMALLOC_H
#define GOODMALLOC_H

#include <stdlib.h>

void *createMem(size_t size);
void *createList(size_t size, char *name);
int assignVal(char *name, int index, int val);
void *freeElem(char *name);
void *freeElem();

#endif
