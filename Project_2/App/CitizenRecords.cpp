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
    next = NULL;
}

InfoNode::~InfoNode() {

}

InfoList::InfoList() {
    head = NULL;
}

void InfoList::setParameters(int pNum){
    processNum = pNum;
    entriesNum = 0;
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

string InfoList::getEntry(int place) {
    if (place>entriesNum or place<=0){
        return "-2";
    }
    InfoNode* tmp = head;
    while (place!=1){
        tmp = tmp->next;
        place--;
    }
    return tmp->info;
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

void InfoList::increment() {
    entriesNum++;
}

int InfoList::getCapacity() {
    return entriesNum;
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

InfoTable::InfoTable(int pTotalNum) {
    table = new InfoList[pTotalNum];
    tableSize = pTotalNum;
    for (int i=0;i<pTotalNum;i++){
        table[i].setParameters(i);
    }
}

void InfoTable::insertNode(int pNum, string s1) {
    for (int i=0;i<tableSize;i++){
        if (table[i].processNum == pNum){
            table[i].insertNode(s1);
            table[i].entriesNum++;
        }
    }
}

int InfoTable::getCapacity(int pNum) {
    for (int i=0;i<tableSize;i++){
        if (table[i].processNum == pNum){
            return table[i].entriesNum;
        }
    }
    return -1;
}

string InfoTable::getEntry(int pNum, int place) {
    for (int i=0;i<tableSize;i++){
        if (table[i].processNum == pNum){
            return table[i].getEntry(place);
        }
    }
    return "-1";
}

int InfoTable::getInfo(string s1) {
    string* tmp;
    for (int i=0;i<tableSize;i++){
        tmp = table[i].getInfo(s1);
        if (tmp != NULL){
            return i;
        }
    }
    return -1;
}

InfoTable::~InfoTable() {
    delete[] table;
}