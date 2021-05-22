#include "CitizenRecords.h"
#include "InputHandler.h"

class SkipListNode{
private:
    Record* data;
    string date;
    SkipListNode** next;
public:
    SkipListNode(Record*,const string&,int);
    ~SkipListNode();
    friend class SkipList;
};

class SkipList{
private:
    int currentLevel;
    int maxLevel;
    SkipListNode* head;
public:
    SkipList(int);
    void insert(Record*,const string&);
    string getDate(int);
    bool inList(int);
    ~SkipList();
};

class VirusSkipListNode{
private:
    string virus;
    string isVaccinated;
    SkipList* sList;
    VirusSkipListNode* next;
public:
    VirusSkipListNode(const string&,const string&,int);
    ~VirusSkipListNode();
    friend class VirusSkipList;
};

class VirusSkipList{
private:
    int levels;
    VirusSkipListNode* head;
public:
    VirusSkipList(int);
    void insert(const string&,const string&,Record*,const string&);
    string getVaccinateInfo(int,const string&);
    string getVaccinateInfo(int);
    ~VirusSkipList();
};