#include <string>
using namespace std;

class SkipListNode{
private:
    int id;
    string date;
    SkipListNode** next;
public:
    SkipListNode(int,const string&,int);
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
    void insert(int,const string&);
    void display();
    ~SkipList();
};