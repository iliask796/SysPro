#include "SkipList.h"
#include <iostream>
#include <ctime>
#include <stdlib.h>

SkipListNode::SkipListNode(int id1, const string& date1, int level) {
    id = id1;
    date = date1;
    next = new SkipListNode*[level+1];
    for (int i=0;i<level;i++){
        next[i] = NULL;
    }
}

SkipListNode::~SkipListNode() {
    delete[] next;
}

SkipList::SkipList(int levels) {
    currentLevel = 0;
    maxLevel = --levels;
    head = new SkipListNode(-9999," ",maxLevel);
    srand(time(NULL));
}

void SkipList::insert(int id1,const string& date1) {
    SkipListNode* tmp = head;
    SkipListNode* NodesArray[maxLevel+1];
    int i;
    for (i=0;i<=maxLevel;i++){
        NodesArray[i] = NULL;
    }
    i = currentLevel;
    while (i>=0){
        while (tmp->next[i] != NULL && tmp->next[i]->id < id1){
            tmp = tmp->next[i];
        }
        NodesArray[i]=tmp;
        i--;
    }
    if (tmp->next[0] == NULL || tmp->next[0]->id != id1){
        int nodeLevel = 0;
        int coin = rand() % 2 + 1;
        while (coin == 2 && nodeLevel < maxLevel){
            nodeLevel++;
            coin = rand() % 2 + 1;
        }
        if (nodeLevel > currentLevel){
            i = currentLevel;
            while (i < nodeLevel){
                NodesArray[++i] = head;
            }
            currentLevel = nodeLevel;
        }
        SkipListNode* new_node = new SkipListNode(id1,date1,nodeLevel);
        for (i=0;i<=nodeLevel;i++){
            new_node->next[i] = NodesArray[i]->next[i];
            NodesArray[i]->next[i] = new_node;
        }
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