#include "SkipList.h"
#include <iostream>
#include <ctime>
#include <stdlib.h>

SkipListNode::SkipListNode(int id1, const string& date1, int level) {
    id = id1;
    date = date1;
    next = new SkipListNode*[level+1];
    for (int i=0;i<=level;i++){
        next[i] = NULL;
    }
}

SkipListNode::~SkipListNode() {
    delete[] next;
}

SkipList::SkipList(int levels) {
    currentLevel = 0;
    maxLevel = --levels;
    head = new SkipListNode(-9999,"-1",maxLevel);
    srand(time(NULL));
}

void SkipList::insert(int id1,const string& date1) {
    SkipListNode* tmp = head;
    //create an array and store the addresses of the previous nodes of each level
    SkipListNode* NodesArray[maxLevel+1];
    int i;
    for (i=0;i<=maxLevel;i++){
        NodesArray[i] = NULL;
    }
    i = currentLevel;
    while (i>=0){
        while (tmp->next[i] != NULL and tmp->next[i]->id < id1){
            tmp = tmp->next[i];
        }
        NodesArray[i--]=tmp;
    }
    if (tmp->next[++i] == NULL or tmp->next[i]->id != id1){
        //determine the levels of the new node
        int nodeLevel = 0;
        int coin = rand() % 2 + 1;
        while (coin == 2 and nodeLevel < maxLevel){
            nodeLevel++;
            coin = rand() % 2 + 1;
        }
        //update list's levels
        if (nodeLevel > currentLevel){
            i = currentLevel;
            while (i < nodeLevel){
                NodesArray[++i] = head;
            }
            currentLevel = nodeLevel;
        }
        //create and insert the new node
        SkipListNode* new_node = new SkipListNode(id1,date1,nodeLevel);
        for (i=0;i<=nodeLevel;i++){
            new_node->next[i] = NodesArray[i]->next[i];
            NodesArray[i]->next[i] = new_node;
        }
    }
}

string SkipList::getDate(int id1) {
    SkipListNode* tmp = head;
    int i=currentLevel;
    while (i>=0){
        while (tmp->next[i] != NULL and tmp->next[i]->id < id1){
            tmp = tmp->next[i];
        }
        i--;
    }
    if (tmp->next[++i] != NULL and tmp->next[i]->id == id1){
        return tmp->next[i]->date;
    }
    else{
        return "-2";
    }
}

void SkipList::remove(int id1) {
    SkipListNode* tmp = head;
    //create an array and store the addresses of the previous nodes of each level
    SkipListNode* NodesArray[maxLevel+1];
    int i;
    for (i=0;i<=maxLevel;i++){
        NodesArray[i] = NULL;
    }
    i = currentLevel;
    while (i >= 0){
        while (tmp->next[i] != NULL and tmp->next[i]->id < id1){
            tmp = tmp->next[i];
        }
        NodesArray[i--] = tmp;
    }
    //connect required previous nodes with the ones after the node we remove
    tmp = tmp->next[0];
    if (tmp != NULL and tmp->id == id1){
        while (++i <= currentLevel){
            if (NodesArray[i]->next[i] != tmp){
                break;
            }
            NodesArray[i]->next[i] = tmp->next[i];
        }
        //TODO: delete node
    }
    //reduce empty levels
    while (head->next[currentLevel] == NULL and  currentLevel > 0){
        currentLevel--;
    }
}

void SkipList::display() {
    cout << "Printing Skip List:" << endl;
    for (int i=0;i<=currentLevel;i++){
        SkipListNode* tmp = head->next[i];
        cout << "Level " << i << ": ";
        while (tmp != NULL){
            cout << tmp->id << " ";
            tmp = tmp->next[i];
        }
        cout << endl;
    }
}

SkipList::~SkipList() {
    if (head->next[0]!=NULL){
        SkipListNode* tmp = head->next[0];
        while (tmp->next[0] != NULL){
            delete head;
            head = tmp;
            tmp = head->next[0];
        }
    }
    delete head;
}