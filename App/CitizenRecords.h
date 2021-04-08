#include "HashUtil.h"

class Record{
private:
    int id;
    string name;
    string* country;
    int age;
public:
    Record(int,string,string*,int);
    int getId() const;
    const string& getName() const;
    string *getCountry() const;
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
    bool searchNode(int);
    Record* getNode(int);
    void print();
    ~RecordList();
};

class RecordTable{
private:
    int size;
    RecordList* table;
public:
    RecordTable(int);
    Record* insertElement(int,string,string*,int);
    Record* getEntry(int);
    void displayTable();
    ~RecordTable();
};

class InfoNode{
private:
    string info;
    InfoNode* next;
public:
    InfoNode(string);
    ~InfoNode();
    friend class InfoList;
};

class InfoList{
private:
    InfoNode* head;
public:
    InfoList();
    void insertNode(string);
    string* getInfo(string);
    void displayList();
    ~InfoList();

};