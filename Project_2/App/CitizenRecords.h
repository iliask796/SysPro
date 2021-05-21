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
    string* getCountry() const;
    int getAge() const;
    ~Record();
};

class RecordNode{
private:
    Record* info;
    RecordNode* next;
public:
    RecordNode(Record*);
    ~RecordNode();
    friend class RecordList;
};

class RecordList{
private:
    RecordNode* head;
public:
    RecordList();
    void insertNode(Record*);
    Record* getNode(int);
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
    int processNum;
    int entriesNum;
    InfoNode* head;
public:
    InfoList();
    void setParameters(int);
    void insertNode(string);
    string getEntry(int);
    string* getInfo(string);
    void increment();
    int getCapacity();
    ~InfoList();
    friend class InfoTable;
};

class InfoTable{
private:
    int tableSize;
    InfoList* table;
public:
    InfoTable(int);
    void insertNode(int,string);
    int getCapacity(int);
    string getEntry(int,int);
    int getInfo(string);
    ~InfoTable();
};