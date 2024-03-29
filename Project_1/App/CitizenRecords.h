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
    int population;
    InfoNode* next;
public:
    InfoNode(string);
    void increasePopulation();
    int getPopulation();
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
    void increasePopulation(string);
    int getPopulation(string);
    int getCapacity();
    string getElement(int);
    ~InfoList();
};