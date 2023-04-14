#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include "goodmalloc.hpp"

#define MEM_SIZE 250*1024*1024 // 250 MB
#define LIST_SIZE 50
#define RANDOM_INT_RANGE 100000

using namespace std;

void merge(size_t leftSize, list *left, size_t rightSize, list *right, list *l){
    size_t i=0, j=0, k=0;
    while(i < leftSize && j < rightSize){
        if(getVal(left, i+1) < getVal(right, j+1)){
            assignVal("list1", k+1, getVal(left, i+1));
            k++;
            i++;
        }
        else{
            assignVal("list1", k+1, getVal(right, j+1));
            k++;
            j++;
        }
    }
    while(i < leftSize){
        assignVal("list1", k+1, getVal(left, i+1));
        k++;
        i++;
    }
    while(j < rightSize){
        assignVal("list1", k+1, getVal(right, j+1));
        k++;
        j++;
    }
}

void mergesort(list *l){
    initScope();
    if(l->size <= 1){
        return;
    }

    // create two lists
    size_t leftSize = l->size / 2;
    size_t rightSize = l->size - leftSize;

    string leftName = "left" + to_string(leftSize);
    string rightName = "right" + to_string(rightSize);

    list *left = (list *)createList(leftSize, leftName.c_str());
    list *right = (list *)createList(rightSize, rightName.c_str());

    // copy elements to left and right
    int idx = l->head;
    for(int i=0; i<leftSize; i++){
        assignVal(leftName.c_str(), i+1, getVal(l, i+1));
    }
    for(int i=0; i<rightSize; i++){
        assignVal(rightName.c_str(), i+1, getVal(l, i+1+leftSize));
    }

    // sort left and right
    mergesort(left);
    mergesort(right);

    // merge left and right
    merge(leftSize, left, rightSize, right, l);

    endScope();
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
    for(int i=0; i<LIST_SIZE; i++){
        assignVal("list1", i+1, rand() % RANDOM_INT_RANGE + 1);
    }
    cout << "List creation successful" << endl;

    // sort the list
    mergesort(l);

    // print the list
    int idx = l->head;
    for(int i=0; i<LIST_SIZE; i++){
        cout << getVal(l, idx) << " ";
    }
    cout << endl;

    endScope();
}