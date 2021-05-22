#include <string>
using namespace std;

class TravelDataNode{
private:
    string* country;
    string date;
    TravelDataNode* next;
public:
    TravelDataNode(string*,const string&);
    ~TravelDataNode();
    friend class TravelDataList;
};

class TravelDataList{
private:
    string virus;
    TravelDataNode* head;
    TravelDataList* next;
public:
    TravelDataList(const string&);
    void insertNode(string*,const string&);
    int getRequests();
    int getRequests(const string&);
    ~TravelDataList();
    friend class TravelDataTable;
};

class TravelDataTable{
private:
    int totalRequests;
    TravelDataList* table;
public:
    TravelDataTable();
    void insertNode(const string&,string*,const string&);
    int getRequests(const string&);
    int getRequests(const string&,const string&);
    int getTotalRequests();
    ~TravelDataTable();
};
