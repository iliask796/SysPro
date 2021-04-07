#include "SkipList.h"
#include <iostream>
#include <ctime>
#include <stdlib.h>

SkipListNode::SkipListNode(Record* record, const string& date1, int level) {
    data = record;
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
    head = new SkipListNode(NULL,"-1",maxLevel);
    srand(time(NULL));
}

void SkipList::insert(Record* data1,const string& date1) {
    SkipListNode* tmp = head;
    //create an array and store the addresses of the previous nodes of each level
    SkipListNode* NodesArray[maxLevel+1];
    int i;
    for (i=0;i<=maxLevel;i++){
        NodesArray[i] = NULL;
    }
    i = currentLevel;
    while (i>=0){
        while (tmp->next[i] != NULL and tmp->next[i]->data->getId() < data1->getId()){
            tmp = tmp->next[i];
        }
        NodesArray[i--]=tmp;
    }
    if (tmp->next[++i] == NULL or tmp->next[i]->data->getId() != data1->getId()){
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
        SkipListNode* new_node = new SkipListNode(data1,date1,nodeLevel);
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
        while (tmp->next[i] != NULL and tmp->next[i]->data->getId() < id1){
            tmp = tmp->next[i];
        }
        i--;
    }
    if (tmp->next[++i] != NULL and tmp->next[i]->data->getId() == id1){
        return tmp->next[i]->date;
    }
    else{
        return "-2";
    }
}

bool SkipList::inList(int id1) {
    SkipListNode* tmp = head;
    int i=currentLevel;
    while (i>=0){
        while (tmp->next[i] != NULL and tmp->next[i]->data->getId() < id1){
            tmp = tmp->next[i];
        }
        i--;
    }
    if (tmp->next[++i] != NULL and tmp->next[i]->data->getId() == id1){
        return true;
    }
    else{
        return false;
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
        while (tmp->next[i] != NULL and tmp->next[i]->data->getId() < id1){
            tmp = tmp->next[i];
        }
        NodesArray[i--] = tmp;
    }
    //connect required previous nodes with the ones after the node we remove
    tmp = tmp->next[0];
    if (tmp != NULL and tmp->data->getId() == id1){
        while (++i <= currentLevel){
            if (NodesArray[i]->next[i] != tmp){
                break;
            }
            NodesArray[i]->next[i] = tmp->next[i];
        }
        delete tmp;
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
            cout << tmp->data->getId() << " ";
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

VirusSkipListNode::VirusSkipListNode(const string& virus1, const string& result, int levels) {
    virus=virus1;
    isVaccinated=result;
    sList = new SkipList(levels);
    next = NULL;
}

VirusSkipListNode::~VirusSkipListNode() {
    delete sList;
}

VirusSkipList::VirusSkipList(int levels1) {
    levels = levels1;
    head = NULL;
}

void VirusSkipList::insert(const string& virus1, const string& result, Record* data1, const string& date1) {
    VirusSkipListNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->isVaccinated==result and tmp->virus==virus1){
            tmp->sList->insert(data1,date1);
            return;
        }
        tmp = tmp->next;
    }
    VirusSkipListNode* new_node = new VirusSkipListNode(virus1,result,levels);
    new_node->sList->insert(data1,date1);
    VirusSkipListNode* new_node1;
    if (result == "YES"){
        new_node1 = new VirusSkipListNode(virus1,"NO",levels);
    }
    else{
        new_node1 = new VirusSkipListNode(virus1,"YES",levels);
    }
    new_node->next = new_node1;
    tmp = head;
    new_node1->next = tmp;
    head = new_node;
}

string VirusSkipList::getDate(const string& virus1, int date1) {
    VirusSkipListNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->isVaccinated=="YES" and tmp->virus==virus1){
            return tmp->sList->getDate(date1);
        }
        tmp = tmp->next;
    }
    return "-1";
}

int VirusSkipList::isNotVaccinated(const string& virus1, int id1) {
    VirusSkipListNode* tmp = head;
    while (tmp!=NULL) {
        if (tmp->isVaccinated == "NO" and tmp->virus == virus1) {
            if (tmp->sList->inList(id1)==true){
                return 1;
            }
            else{
                return 0;
            }
        }
        tmp = tmp->next;
    }
    return -1;
}

void VirusSkipList::display() {
    if (head!=NULL){
        VirusSkipListNode* tmp = head;
        while (tmp!=NULL){
            cout << "Printing: "<< tmp->virus << " + " << tmp->isVaccinated << endl;
            tmp->sList->display();
            tmp=tmp->next;
        }
    }
}

VirusSkipList::~VirusSkipList() {
    if (head==NULL){
        return;
    }
    VirusSkipListNode* tmp = head->next;
    while (tmp != NULL)
    {
        delete head;
        head = tmp;
        tmp = head->next;
    }
    delete head;
}