#include "goodmalloc.hpp"
#include <iostream>
#include <map>
#include <string>
#include <stack>

using namespace std;

// struct to represent element
typedef struct element{
    int val;
    int prev; // offset value
    int next; // offset value
}element;

#define PAGE_SIZE sizeof(element)

// struct to represent a list
typedef struct list{
    int head; // offset
    int tail; // offset
    int size;
}list;

typedef struct mem_{ // struct to represent memory
    void* head;
    void* tail;
    int size;
    char *used;
}mem_;

mem_ *mem;
map<string, int> symbolTable;
stack<int> st;

int getOffset(void *ptr){
    return (int *)ptr - (int *)mem->head;
}

void initScope(){
    st.push(-1);
    return;
}

void endScope(){
    int top = st.top();
    st.pop();
    while(top != -1){
        int off = top;
        while(mem->used[off / PAGE_SIZE] == 1){
            mem->used[off / PAGE_SIZE] = 0;
            off = ((element *)((int *)mem->head + off))->next;
        }

        top = st.top();
        st.pop();
    }
    return;
}

void *createMem(size_t size){
    mem = (mem_ *)malloc(sizeof(mem_));

    int num_pages = size / PAGE_SIZE;
    mem->used = (char *)malloc(num_pages * sizeof(char));
    mem->size = num_pages;

    void *ptr_ = malloc(size);
    mem->head = ptr_;
    mem->tail = (void *)((int *)ptr_ + size);

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
                if(cnt == 0)
                    break;
            }
        }
        if(cnt != 0){
            printf("Not enough memory to create list\n");
            exit(1);
        }
    }

    // use first fit algorithm to allocate memory
    mem->used[start_index] = 1;
    list *l = (list *)start;
    l->size = size;
    int cnt = size;

    int curr=-1, prev=-1, first=-1;
    for(int i=0; i<mem->size; i++){
        if(mem->used[i] == 0){
            mem->used[i] = 1;
            if(first == -1){
                first = i;
            }
            curr = i;
            element *e = (element *)((int *)mem->head + curr*PAGE_SIZE);
            e->val = 0;
            if(prev != -1)
                e->prev = prev;
            element *e_prev = (element *)((int *)mem->head + prev*PAGE_SIZE);
            e_prev->next = curr;
            cnt--;
            if(cnt == 0){
                break;
            }
        }
    }
    element *e_head = (element *)((int *)mem->head + first*PAGE_SIZE);
    e_head->prev = curr;
    element *e_tail = (element *)((int *)mem->head + curr*PAGE_SIZE);
    e_tail->next = first;

    // add to symbol table
    string s(name);
    symbolTable[s] = getOffset(start);
    return start;
}

int assignVal(char *name, int index, int val){
    // index specifies the index of the element in the list (1 based)

    // get the offset of the list from symbol table
    int offset = -1;
    string s(name);
    if(symbolTable.find(s) != symbolTable.end()){
        offset = symbolTable[s];
    }
    else{
        printf("List not found\n");
        exit(1);
    }

    list *l = (list *)((int *)mem->head + offset);
    if(index > l->size){
        printf("Index out of bounds\n");
        exit(1);
    }

    for(int i=0; i<index; i++){
        element *e = (element *)((int *)mem->head + offset);
        offset = e->next;
    }

    element *e = (element *)((int *)mem->head + offset);
    e->val = val;

    return 0;
}

void freeElem(char *name){
    // get the offset of the list from symbol table
    int offset = -1;
    string s(name);
    if(symbolTable.find(s) != symbolTable.end()){
        offset = symbolTable[s];
    }
    else{
        printf("List not found\n");
        exit(1);
    }

    // mark cells as free
    int off = symbolTable[s];
    list *l = (list *)((int *)mem->head + off);
    while(mem->used[off / PAGE_SIZE] == 1){
        mem->used[off / PAGE_SIZE] = 0;
        off = ((element *)((int *)mem->head + off))->next;
    }

    // remove from symbol table
    symbolTable.erase(s);

    return;
}

void freeElem(){
    endScope();
    return;
}