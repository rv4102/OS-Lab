#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include "goodmalloc.hpp"

#define MEM_SIZE 250*1024*1024 // 250 MB
#define LIST_SIZE 1000
#define RANDOM_INT_RANGE 100000

using namespace std;

list* sort(list* head, int depth){
    if(head->size <= 1){
        return head;
    }
    initScope();
    int n = head->size;
    int mid = n/2;
    string lname = "left"+to_string(depth);
    string rname = "right"+to_string(depth);
    list* left = (list*)createList(mid, lname.c_str());
    list* right = (list*)createList(n-mid, rname.c_str());
    int i = 0;
    int j = 0;
    int k = 0;
    element* lcurr = (element*)getAddr(left->head);
    element* rcurr = (element*)getAddr(right->head);
    element* curr = (element*)getAddr(head->head);
    while(i < mid){
        // assignVal(lname.c_str(), i+1, getVal(head, i+1));
        lcurr->val = curr->val;
        lcurr = (element*)getAddr(lcurr->next);
        curr = (element*)getAddr(curr->next);
        i++;
    }
    while(j < n-mid){
        // assignVal(rname.c_str(), j+1, getVal(head, mid+j+1));
        rcurr->val = curr->val;
        rcurr = (element*)getAddr(rcurr->next);
        curr = (element*)getAddr(curr->next);
        j++;
    }
    left = sort(left, depth+1);
    right = sort(right, depth+1);
    i = 0;
    j = 0;
    k = 0;
    string resname = "res"+to_string(depth);
    list* res = (list*)createList(n, resname.c_str());
    element* rescurr = (element*)getAddr(res->head);
    lcurr = (element*)getAddr(left->head);
    rcurr = (element*)getAddr(right->head);
    while(i < left->size && j < right->size){
        if(lcurr->val < rcurr->val){
            rescurr->val = lcurr->val;
            lcurr = (element*)getAddr(lcurr->next);
            i++;
        }
        else{
            rescurr->val = rcurr->val;
            rcurr = (element*)getAddr(rcurr->next);
            j++;
        }
        rescurr = (element*)getAddr(rescurr->next);
        k++;
    }
    while(i < left->size){
        rescurr->val = lcurr->val;
        lcurr = (element*)getAddr(lcurr->next);
        rescurr = (element*)getAddr(rescurr->next);
        i++;
        k++;
    }
    while(j < right->size){
        rescurr->val = rcurr->val;
        rcurr = (element*)getAddr(rcurr->next);
        rescurr = (element*)getAddr(rescurr->next);
        j++;
        k++;
    }
    // while(i < left->size && j < right->size){
    //     if(getVal(left, i+1) < getVal(right, j+1)){
    //         assignVal(resname.c_str(), k+1, getVal(left, i+1));
    //         i++;
    //     }
    //     else{
    //         assignVal(resname.c_str(), k+1, getVal(right, j+1));
    //         j++;
    //     }
    //     k++;
    // }
    // while(i < left->size){
    //     assignVal(resname.c_str(), k+1, getVal(left, i+1));
    //     i++;
    //     k++;
    // }
    // while(j < right->size){
    //     assignVal(resname.c_str(), k+1, getVal(right, j+1));
    //     j++;
    //     k++;
    // }
    endScope();
    return res;
}
int main(){
    initScope();
    void *ptr = createMem(MEM_SIZE);
    if(ptr == NULL){
        cout << "Memory allocation failed" << endl;
        return 0;
    }
    cout << "Memory allocation successful" << endl;

    // create a list
    struct timeval start, end;
    gettimeofday(&start, NULL);
    list *l = (list *)createList(LIST_SIZE, "list1");
    if(l == NULL){
        cout << "List creation failed" << endl;
        return 0;
    }
    gettimeofday(&end, NULL);
    cout << "Time taken to create list: " << (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000 << " ms" << endl;
    srand(time(NULL));
    
    gettimeofday(&start, NULL);
    for(int i=0; i<LIST_SIZE; i++){
        int val = rand() % RANDOM_INT_RANGE + 1;
        // dbg(val);
        assignVal("list1", i+1, val);
    }
    gettimeofday(&end, NULL);
    cout << "Time taken to assign values to list: " << (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000 << " ms" << endl;
    printList(l);
    cout << "List creation successful" << endl;
    gettimeofday(&start, NULL);
    l = sort(l, 0);
    gettimeofday(&end, NULL);
    cout << "Time taken to sort list: " << (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000 << " ms" << endl;
    printList(l);
    endScope();
    return 0;
}