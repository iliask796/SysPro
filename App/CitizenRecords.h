#include "BloomFilter.h"

class Record{
private:
    int id;
    string name;
    string country;
    int age;
public:
    Record(int,string,string,int);
    int getId() const;
    const string &getName() const;
    const string &getCountry() const;
    int getAge() const;
    ~Record();
};

class RecordNode{
private:
    Record* info;
    RecordNode* next;
public:
    RecordNode(Record*);
    void displayStats();
    ~RecordNode();
    friend class RecordList;
};

class RecordList{
private:
    RecordNode* head;
public:
    RecordList();
    void insertNode(Record*);
    void print();
    ~RecordList();
};

class RecordTable{
private:
    int size;
    RecordList* table;
public:
    RecordTable(int);
    void insertElement(int, Record*);
    void displayTable();
    ~RecordTable();
};