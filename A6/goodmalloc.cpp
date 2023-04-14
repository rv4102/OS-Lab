#include "goodmalloc.hpp"
#include <iostream>
#include <map>
#include <string>
#include <stack>

using namespace std;

#define PAGE_SIZE sizeof(element)   // 12 bytes

typedef struct page_table_entry{
    int frame;
    int used;
}page_table_entry;  // 8 bytes

typedef struct page_table{
    page_table_entry *entries;
    int size;
}page_table;

page_table *pt;
void *mem_base_ptr;
map<string, int> symbolTable;
stack<int> global_stack;

int getOffset(void *ptr){
    return (char *)ptr - (char *)mem_base_ptr;
}

char *getAddr(int page_table_index){
    return (char *)mem_base_ptr + pt->entries[page_table_index].frame;
}

void initScope(){
    global_stack.push(-1);
    return;
}

void endScope(){
    int top = global_stack.top();
    global_stack.pop();
    while(top != -1){
        int idx = top;
        while(pt->entries[idx].used == 1){
            pt->entries[idx].used = 0;

            // type cast to char * to increment by idx bytes exactly
            idx = ((element *)getAddr(idx))->next;
        }

        top = global_stack.top();
        global_stack.pop();
    }
    return;
}

void printList(list *l)
{
    int idx = l->head;
    int cnt = 0;
    while(cnt < l->size){
        cout << ((element *)getAddr(idx))->val << " ";
        idx = ((element *)getAddr(idx))->next;
        cnt++;
    }
    cout << "\n";
}

void *createMem(size_t size){
    global_stack.push(-2);

    // make size a multiple of page size
    if(size % PAGE_SIZE != 0)
        size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
    
    int num_pages = size / PAGE_SIZE;       // 21845334

    void *ptr_ = malloc(size);
    mem_base_ptr = ptr_;
    printf("mem_base_ptr = %p\n", mem_base_ptr);
    printf("size of page_table = %lu\n", sizeof(page_table));
    printf("size of page_table_entry = %lu\n", sizeof(page_table_entry));   // 8 bytes
    pt = (page_table *)malloc(sizeof(page_table));      // 16 bytes
    printf("pt = %p\n", pt);

    pt->size = num_pages;
    pt->entries = (page_table_entry *)malloc(num_pages * sizeof(page_table_entry));
    for(int i=0; i<num_pages; i++){
        pt->entries[i].frame = i*PAGE_SIZE;     // frames at address 0, 12, 24, 36, ...
        pt->entries[i].used = 0;
    }
    return ptr_;
}

void *createList(size_t size, const char *name){
    // size refers to number of nodes in the list
    void *start = NULL;
    int start_index = -1;
    if(size > pt->size){
        printf("Not enough memory to create list\n");
        exit(1);
    }
    else{
        size_t cnt=size+1; // 1 extra cell needed to keep head tail pointers of list
        for(int i=0; i<pt->size; i++){
            if(pt->entries[i].used == 0){
                if(start == NULL){
                    start_index = i;
                    start = (void *)(getAddr(i));
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
    // dbg(start_index);
    // dbg(start);
    // use first fit algorithm to allocate memory
    pt->entries[start_index].used = 1;
    list *l = (list *)start;    //list is 12 bytes
    l->size = size;
    int cnt = size;

    int curr_node=-1, prev_node=-1, first_node=-1;
    for(int i=0; i<pt->size; i++){
        if(pt->entries[i].used == 0){
            pt->entries[i].used = 1;
            if(first_node == -1){
                first_node = i;
            }
            curr_node = i;
            element *e = (element *)(getAddr(curr_node));
            e->val = 0;
            e->prev = -1;
            e->next = -1;
            if(prev_node != -1){
                e->prev = prev_node;
                element *e_prev = (element *)(getAddr(prev_node));
                e_prev->next = curr_node;
            }
            prev_node = curr_node;
            cnt--;
            if(cnt == 0){
                break;
            }
        }
    }

    l->head = first_node;
    l->tail = curr_node;
    
    element *e_head = (element *)(getAddr(first_node));
    e_head->prev = curr_node;
    element *e_tail = (element *)(getAddr(curr_node));
    e_tail->next = first_node;

    // add to symbol table
    string s(name);
    symbolTable[s] = start_index;

    // add to global stack
    global_stack.push(start_index);

    return start;
}

int assignVal(const char *name, int index, int val){
    // index specifies the index of the element in the list (1 based)

    // get the offset of the list from symbol table
    int idx = -1;
    string s(name);
    if(symbolTable.find(s) != symbolTable.end()){
        idx = symbolTable[s];
    }
    else{
        printf("List not found\n");
        exit(1);
    }

    list *l = (list *)(getAddr(idx));
    if(index > l->size){
        dbg(index);
        dbg(l->size);
        printf("Index out of bounds[assign_val]\n");
        exit(1);
    }

    idx = l->head;
    element *e;
    for(int i=0; i<index; i++){
        e = (element *)(getAddr(idx));
        idx = e->next;
    }
    e->val = val;

    return 0;
}

int getVal(list *l, int index){
    if(index > l->size){
        dbg(index);
        dbg(l->size);
        printf("Index out of bounds[getVal]\n");
        exit(1);
    }

    int page_table_idx = l->head;
    element *e;
    for(int i=0; i<index; i++){
        e = (element *)(getAddr(page_table_idx));
        page_table_idx = e->next;
    }

    return e->val;
}

void freeElem(char *name){
    // get the offset of the list from symbol table
    int idx = -1;
    string s(name);
    if(symbolTable.find(s) != symbolTable.end()){
        idx = symbolTable[s];
    }
    else{
        printf("List not found\n");
        exit(1);
    }

    // mark cells as free
    list *l = (list *)(getAddr(idx));
    while(pt->entries[idx].used == 1){
        pt->entries[idx].used = 0;
        idx = ((element *)getAddr(idx))->next;
    }

    // remove from symbol table
    symbolTable.erase(s);

    return;
}

void freeElem(){
    endScope();
    return;
}