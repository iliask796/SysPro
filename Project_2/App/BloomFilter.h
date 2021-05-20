#include "HashUtil.h"

class BloomFilter{
private:
    int* filter;
    int filterSize;
    int num_hashes;
public:
    BloomFilter(int,int);
    BloomFilter(int,int,int*);
    void addToFilter(string);
    bool probInFilter(string);
    int* getFilter();
    ~BloomFilter();
};

class BloomNode{
private:
    string virus;
    BloomFilter* filter;
    BloomNode* next;
public:
    BloomNode(string,int,int);
    BloomNode(string,int,int,int*);
    ~BloomNode();
    friend class BloomList;
};

class BloomList{
private:
    int filterSize;
    int num_hashes;
    BloomNode* head;
public:
    BloomList(int,int);
    void addToFilter(string,string);
    void addFilter(string,int*);
    int probInFilter(string,string);
    int* getFilter(string);
    ~BloomList();
};