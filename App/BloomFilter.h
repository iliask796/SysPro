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
    BloomNode();
    ~BloomNode();
    friend class BloomList;
};

class BloomList{
private:
    BloomNode* head;
public:
    BloomList();
    ~BloomList();
};