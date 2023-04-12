#include "goodmalloc.hpp"
#include <stdio.h>

// struct to represent element
typedef struct element{
    int val;
    element* prev;
    element* next;
}element;

#define PAGE_SIZE sizeof(element)

// struct to represent a list
typedef struct list{
    element* head;
    element* tail;
    int size;
}list;

typedef struct mem_{ // struct to represent memory
    void* head;
    void* tail;
    int size;
    char *used;
}mem_;

mem_ *mem;

void *createMem(size_t size){
    mem = (mem_ *)malloc(sizeof(mem_));

    int num_pages = size / PAGE_SIZE;
    mem->used = (char *)malloc(num_pages * sizeof(char));
    mem->size = num_pages;

    void *ptr_ = malloc(size);
    mem->head = ptr_;
    mem->tail = (void *)((int *)ptr_ + size); // unsure about this

    return ptr_;
}

void *createList(size_t size, char *name){
    // size refers to number of nodes in the list
    void *start = NULL;
    int start_index = -1;
    if(size > mem->size){
        printf("Not enough memory to create list\n");
        exit(1);
    }
    else{
        size_t cnt=size+1; // 1 extra cell needed to keep head tail pointers of list
        for(int i=0; i<mem->size; i++){
            if(mem->used[i] == 0){
                if(start == NULL){
                    start_index = i;
                    start = (void *)((int *)mem->head + i*PAGE_SIZE);
                }
                cnt--;
            if(cnt == 0){
                break;
        }
        if(cnt != 0){
            printf("Not enough memory to create list\n");
            exit(1);
        }
    }

    // use first fit algorithm to allocate memory
    mem->used[i] = 1;
    list *l = (list *)start;
    l->size = size;
    int cnt = size;
    for(int i=0; i<mem->size && cnt > 0; i++){
        if(mem->used[i] == 0){
            l->head = (element *)(mem->)
        }
    }
}