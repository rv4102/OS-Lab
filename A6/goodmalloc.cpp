#include "goodmalloc.hpp"
#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <queue>

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
queue<int> free_list;
int max_mem_used = 0;

int getOffset(void *ptr){
    return (char *)ptr - (char *)mem_base_ptr;
}

char *getAddr(int page_table_index){
    // type cast to char * to increment by idx bytes exactly
    return (char *)mem_base_ptr + pt->entries[page_table_index].frame;
}

void get_mem_footprint()
{
    cout << "Max memory used: " << max_mem_used << " bytes\n";
    return;
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
        pt->entries[idx].used = 0;
        free_list.push(idx);
        int head = ((list *)getAddr(idx))->head;
        int n = ((list *)getAddr(idx))->size;
        int cnt = 0;
        while(cnt < n){
            pt->entries[head].used = 0;
            free_list.push(head);
            head = ((element *)getAddr(head))->next;
            cnt++;
        }

        map<string, int>::iterator it;
        for(it = symbolTable.begin(); it != symbolTable.end(); it++){
            if(it->second == top){
                symbolTable.erase(it);
                break;
            }
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
    cout<<"[createMem]: size = "<<size<<"\n";
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

    // initialize free list
    for(int i=0; i<num_pages; i++){
        free_list.push(i);
    }

    return ptr_;
}

void *createList(size_t size, const char *name){
    cout<<"[createList]: size = "<<size<<" name = "<<name<<"\n";
    int free_list_size = free_list.size();
    if(size+1 > free_list_size){
        printf("Not enough memory to create list\n");
        exit(1);
    }
    int start_index = free_list.front();
    free_list.pop();
    void* start = (void *)(getAddr(start_index));
    pt->entries[start_index].used = 1;
    list *l = (list *)start;    //list is 12 bytes
    l->size = size;
    int curr_node = -1, prev_node = -1, first_node = -1;
    for(int i=0; i<size; i++){
        curr_node = free_list.front();
        free_list.pop();
        if(first_node == -1){
            first_node = curr_node;
        }
        element* e = (element *)(getAddr(curr_node));
        e->val = 0;
        e->prev = -1;
        e->next = -1;
        if(prev_node != -1){
            e->prev = prev_node;
            element* prev_e = (element *)(getAddr(prev_node));
            prev_e->next = curr_node;
        }
        prev_node = curr_node;
    }
    l->head = first_node;
    l->tail = curr_node;
    element* e = (element *)(getAddr(curr_node));
    e->next = first_node;   
    e = (element *)(getAddr(first_node));
    e->prev = curr_node;
    symbolTable[name] = start_index;
    global_stack.push(start_index);
    int used_pages = pt->size - free_list.size();
    int mem_used = used_pages * PAGE_SIZE;
    if(mem_used > max_mem_used)
        max_mem_used = mem_used;
    return start;
}

int assignVal(const char *name, int index, int val){
    cout<<"[assignVal]: name = "<<name<<" index = "<<index<<" val = "<<val<<"\n";
    // index specifies the index of the element in the list (1 based)
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

int assignVal(list *l, int index, int val){
    int idx = -1;
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
    cout<<"[freeElem]: name = "<<name<<"\n";
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
    pt->entries[idx].used = 0;
    free_list.push(idx);
    int head = ((list *)getAddr(idx))->head;
    int n = ((list *)getAddr(idx))->size;
    int cnt = 0;
    while(cnt < n){
        pt->entries[head].used = 0;
        free_list.push(head);
        head = ((element *)getAddr(head))->next;
        cnt++;
    }
    symbolTable.erase(s);
    return;
}

void freeElem(){
    cout<<"[freeElem]:\n";
    endScope();
    return;
}