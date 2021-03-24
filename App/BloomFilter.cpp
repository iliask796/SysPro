#include "BloomFilter.h"
#include <iostream>

BloomFilter::BloomFilter(int sizeInBytes, int no_hashes) {
    filterSize = sizeInBytes / 4;
    num_hashes = no_hashes;
    filter = new int[filterSize];
    for (int i=0;i<filterSize;i++){
        filter[i]=0;
    }
}

void BloomFilter::addToFilter(string str) {
    unsigned long bit;
    for (int i=0;i<num_hashes;i++){
        bit = hash_i(str,i);
        bit %= filterSize * 32;
        filter[bit/32] |= 1 << (bit%32);
    }
}

bool BloomFilter::probInFilter(string str) {
    unsigned long bit;
    for (int i=0;i<num_hashes;i++){
        bit= hash_i(str,i);
        bit %= filterSize * 32;
        if ( (filter[bit/32] & (1 << (bit%32))) == 0 ) {
            return false;
        }
    }
    return true;
}

BloomFilter::~BloomFilter() {
    delete[] filter;
}

BloomNode::BloomNode(string virusName,int size, int no_hashes) {
    virus = virusName;
    filter = new BloomFilter(size,no_hashes);
    next = NULL;
}

BloomNode::~BloomNode() {
    delete filter;
}

BloomList::BloomList(int size, int no_hashes) {
    filterSize = size;
    num_hashes = no_hashes;
    head = NULL;
}

void BloomList::addToFilter(string virusName, string str) {
    BloomNode* tmp = head;
    while (tmp!=NULL){
        if (tmp->virus == virusName){
            tmp->filter->addToFilter(str);
            return;
        }
        tmp = tmp->next;
    }
    BloomNode* new_node = new BloomNode(virusName,filterSize,num_hashes);
    new_node->filter->addToFilter(str);
    tmp = head;
    new_node->next = tmp;
    head = new_node;
}

int BloomList::probInFilter(string virusName, string str) {
    BloomNode* tmp = head;
    while (tmp!=NULL)
    {
        if (tmp->virus == virusName){
            if (tmp->filter->probInFilter(str)){
                return 1;
            }
            else{
                return 0;
            }
        }
        tmp = tmp->next;
    }
    return 2;
}

BloomList::~BloomList() {
    if (head==NULL){
        return;
    }
    BloomNode* tmp = head->next;
    while (tmp != NULL)
    {
        delete head;
        head = tmp;
        tmp = head->next;
    }
    delete head;
}
