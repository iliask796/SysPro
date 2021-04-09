#include <iostream>
#include "CitizenRecords.h"

Record::Record(int id1, string nam, string* country1, int age1) {
    id = id1;
    name = nam;
    country = country1;
    age = age1;
}

int Record::getId() const {
    return id;
}

const string& Record::getName() const {
    return name;
}

string* Record::getCountry() const {
    return country;
}

int Record::getAge() const {
    return age;
}

Record::~Record(){

}

RecordNode::RecordNode(Record* r1){
    info = r1;
    next = NULL;
}

RecordNode::~RecordNode() {
    delete info;
}

RecordList::RecordList() {
    head = NULL;
}

void RecordList::insertNode(Record* r1) {
    RecordNode* new_node = new RecordNode(r1);
    if (head==NULL){
        head = new_node;
        return;
    }
    RecordNode* tmp = head;
    while (tmp->next!=NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

Record* RecordList::getNode(int id) {
    RecordNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->info->getId() == id){
            return tmp->info;
        }
        tmp = tmp->next;
    }
    return NULL;
}

RecordList::~RecordList() {
    if (head==NULL){
        return;
    }
    RecordNode* tmp = head->next;
    while (tmp != NULL)
    {
        delete head;
        head = tmp;
        tmp = head->next;
    }
    delete head;
}

RecordTable::RecordTable(int cap) {
    bool flag = false;
    if (cap==0 or cap==1){
        cap+=2;
    }
    if (cap%2==0){
        cap++;
    }
    while (!flag){
        flag = true;
        for(int i=3;i*i<cap/2;i+=2){
            if (cap%i==0){
                flag = false;
                break;
            }
        }
        if (flag){
            size = cap;
        }
        else{
            cap+=2;
        }
    }
    size = cap;
    table = new RecordList[size];
}

Record* RecordTable::insertElement(int id1, string nam, string* country1, int age1) {
    Record* citizenInfo = new Record(id1,nam,country1,age1);
    int key = citizenInfo->getId();
    unsigned long index = hash_i(to_string(key),0) % size;
    table[index].insertNode(citizenInfo);
    return citizenInfo;
}

Record* RecordTable::getEntry(int id) {
    unsigned long index = hash_i(to_string(id),0) % size;
    return table[index].getNode(id);
}

RecordTable::~RecordTable() {
    delete[] table;
}

InfoNode::InfoNode(string s1) {
    info = s1;
    population = 0;
    next = NULL;
}

void InfoNode::increasePopulation() {
    population++;
}

int InfoNode::getPopulation() {
    return population;
}

InfoNode::~InfoNode() {

}

InfoList::InfoList() {
    head = NULL;
}

void InfoList::insertNode(string s1) {
    InfoNode* new_node = new InfoNode(s1);
    if (head==NULL){
        head = new_node;
        return;
    }
    InfoNode* tmp = head;
    while (tmp->next!=NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

string* InfoList::getInfo(string s1) {
    InfoNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->info.compare(s1)==0){
            return &(tmp->info);
        }
        tmp = tmp->next;
    }
    return NULL;
}

void InfoList::increasePopulation(string s1) {
    InfoNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->info.compare(s1)==0){
            tmp->increasePopulation();
        }
        tmp = tmp->next;
    }
}

int InfoList::getPopulation(string s1) {
    InfoNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->info.compare(s1)==0){
            return tmp->getPopulation();
        }
        tmp = tmp->next;
    }
    return -1;
}

int InfoList::getCapacity() {
    InfoNode* tmp = head;
    int counter = 0;
    while (tmp!=NULL){
        counter++;
        tmp = tmp->next;
    }
    return counter;
}

string InfoList::getElement(int pos) {
    InfoNode* tmp = head;
    if (tmp == NULL){
        return "";
    }
    int i = 1;
    while (pos != i){
        tmp = tmp->next;
        i++;
    }
    return tmp->info;
}

InfoList::~InfoList() {
    if (head==NULL){
        return;
    }
    InfoNode* tmp = head->next;
    while (tmp != NULL)
    {
        delete head;
        head = tmp;
        tmp = head->next;
    }
    delete head;
}