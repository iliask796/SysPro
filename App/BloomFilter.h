#include "HashUtil.h"

class BloomFilter{
private:
    int* filter;
    int filterSize;
    int num_hashes;
public:
    BloomFilter(int,int);
    void addToFilter(unsigned char*);
    bool probInFilter(unsigned char*);
    ~BloomFilter();
};

class BloomNode{
private:
    string virus;
    BloomFilter* filter;
    BloomNode* next;
public:
    BloomNode(string,int,int);
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
    void addToFilter(string,unsigned char*);
    int probInFilter(string,unsigned char*);
    ~BloomList();
};


