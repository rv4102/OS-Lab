#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include "goodmalloc.hpp"

#define MEM_SIZE 250*1024*1024 // 250 MB
#define LIST_SIZE 50000
#define RANDOM_INT_RANGE 100000

using namespace std;

list* sort(list* head, int depth){
    if(head->size <= 1){
        return head;
    }
    int n = head->size;
    int mid = n/2;
    string lname = "left"+to_string(depth);
    string rname = "right"+to_string(depth);
    list* left = (list*)createList(mid, lname.c_str());
    list* right = (list*)createList(n-mid, rname.c_str());
    int i = 0;
    int j = 0;
    int k = 0;
    while(i < mid){
        assignVal(lname.c_str(), i+1, getVal(head, i+1));
        i++;
    }
    while(j < n-mid){
        assignVal(rname.c_str(), j+1, getVal(head, mid+j+1));
        j++;
    }
    left = sort(left, depth+1);
    right = sort(right, depth+1);
    i = 0;
    j = 0;
    k = 0;
    list* res = (list*)createList(n, "res");
    while(i < left->size && j < right->size){
        if(getVal(left, i+1) < getVal(right, j+1)){
            assignVal("res", k+1, getVal(left, i+1));
            i++;
        }
        else{
            assignVal("res", k+1, getVal(right, j+1));
            j++;
        }
        k++;
    }
    while(i < left->size){
        assignVal("res", k+1, getVal(left, i+1));
        i++;
        k++;
    }
    while(j < right->size){
        assignVal("res", k+1, getVal(right, j+1));
        j++;
        k++;
    }
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
    list *l = (list *)createList(LIST_SIZE, "list1");
    if(l == NULL){
        cout << "List creation failed" << endl;
        return 0;
    }
    srand(time(NULL));

    for(int i=0; i<LIST_SIZE; i++){
        int val = rand() % RANDOM_INT_RANGE + 1;
        // dbg(val);
        assignVal("list1", i+1, val);
    }
    // printList(l);
    cout << "List creation successful" << endl;
    l = sort(l, 0);
    // printList(l);
    return 0;

}