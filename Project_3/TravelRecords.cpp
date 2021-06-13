#include "TravelRecords.h"

TravelDataNode::TravelDataNode(string* s1, const string& s2) {
    country = s1;
    date = s2;
    next = NULL;
}

TravelDataNode::~TravelDataNode() {

}

TravelDataList::TravelDataList(const string& s1) {
    virus = s1;
    head = NULL;
    next = NULL;
}

void TravelDataList::insertNode(string* s1, const string& s2) {
    TravelDataNode* new_node = new TravelDataNode(s1,s2);
    if (head==NULL){
        head = new_node;
        return;
    }
    TravelDataNode* tmp = head;
    while (tmp->next!=NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

int TravelDataList::getRequests(const string& date1,const string& date2) {
    int counter=0;
    TravelDataNode* tmp = head;
    while(tmp!=NULL){
        if(compareDates(date1,tmp->date)>=0 and compareDates(tmp->date,date2)>=0){
            counter++;
        }
        tmp=tmp->next;
    }
    return counter;
}

int TravelDataList::getRequests(const string& s1,const string& date1,const string& date2) {
    int counter=0;
    TravelDataNode* tmp = head;
    while(tmp!=NULL){
        if(s1==*tmp->country and compareDates(date1,tmp->date)>=0 and compareDates(tmp->date,date2)>=0){
            counter++;
        }
        tmp=tmp->next;
    }
    return counter;
}

TravelDataList::~TravelDataList() {
    if (head==NULL){
        return;
    }
   TravelDataNode* tmp = head->next;
    while (tmp != NULL)
    {
        delete head;
        head = tmp;
        tmp = head->next;
    }
    delete head;
}

TravelDataTable::TravelDataTable() {
    totalRequests=0;
    table = NULL;
}

void TravelDataTable::insertNode(const string& s1, string* s2, const string& s3) {
    TravelDataList* tmp = table;
    while (tmp!=NULL){
        if (tmp->virus == s1){
            tmp->insertNode(s2,s3);
            totalRequests++;
            return;
        }
        tmp=tmp->next;
    }
    TravelDataList* new_list = new TravelDataList(s1);
    new_list->insertNode(s2,s3);
    totalRequests++;
    if (table == NULL){
        table = new_list;
    }
    else{
        tmp=table;
        while(tmp->next!=NULL){
            tmp=tmp->next;
        }
        tmp->next=new_list;
    }
}

int TravelDataTable::getRequests(const string& s1,const string& date1,const string& date2) {
    TravelDataList* tmp = table;
    while (tmp!=NULL){
        if (tmp->virus == s1){
            return tmp->getRequests(date1,date2);
        }
        tmp=tmp->next;
    }
    return 0;
}

int TravelDataTable::getRequests(const string& s1, const string& s2,const string& date1,const string& date2) {
    TravelDataList* tmp = table;
    while (tmp!=NULL){
        if (tmp->virus == s1){
            return tmp->getRequests(s2,date1,date2);
        }
        tmp=tmp->next;
    }
    return 0;
}

int TravelDataTable::getTotalRequests() {
    return totalRequests;
}

TravelDataTable::~TravelDataTable() {
    if (table==NULL){
        return;
    }
    TravelDataList* tmp = table->next;
    while (tmp != NULL)
    {
        delete table;
        table = tmp;
        tmp = table->next;
    }
    delete table;
}