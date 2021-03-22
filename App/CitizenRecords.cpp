#include <iostream>
#include "CitizenRecords.h"

Record::Record(int x, string nam, string co, int ag) {
    id = x;
    name = nam;
    country = co;
    age = ag;
}

int Record::getId() const {
    return id;
}

const string &Record::getName() const {
    return name;
}

const string &Record::getCountry() const {
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

void RecordNode::displayStats(){
    cout<<info->getId()<<" "<<info->getName()<<" "<<info->getCountry()<<" "<<info->getAge()<<endl;
}

RecordNode::~RecordNode() {

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

bool RecordList::searchNode(int id) {
    RecordNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->info->getId() == id){
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

void RecordList::print(){
    RecordNode* tmp = head;
    while (tmp!=NULL)
    {
        tmp->displayStats();
        tmp = tmp->next;
    }
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

void RecordTable::insertElement(Record* citizenInfo) {
    int key = citizenInfo->getId();
    unsigned long index = hash_i(int_to_charptr(key),0) % size;
    table[index].insertNode(citizenInfo);
}

bool RecordTable::searchElement(int id) {
    unsigned long index = hash_i(int_to_charptr(id),0) % size;
    return table[index].searchNode(id);
}

void RecordTable::displayTable() {
    for (int i=0;i<size;i++){
        cout<<"List number "<<i+1<<" contains:"<<endl;
        table[i].print();
    }
}

RecordTable::~RecordTable() {
    delete[] table;
}