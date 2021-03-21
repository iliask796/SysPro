#include "BloomFilter.h"

BloomFilter::BloomFilter(int sizeInBytes, int no_hashes) {
    filterSize = sizeInBytes / 4;
    num_hashes = no_hashes;
    filter = new int[filterSize];
    for (int i=0;i<filterSize;i++){
        filter[i]=0;
    }
}

void BloomFilter::addToFilter(unsigned char* str) {
    unsigned long bit;
    for (int i=0;i<num_hashes;i++){
        bit = hash_i(str,i);
        bit %= filterSize;
        filter[bit/64] |= 1 << (bit%64);
    }
}

bool BloomFilter::probInFilter(unsigned char* str) {
    unsigned long bit;
    for (int i=0;i<num_hashes;i++){
        bit= hash_i(str,i);
        bit %= filterSize;
        if ( (filter[bit/64] & (1 << (bit%64))) == 0 ) {
            return false;
        }
    }
    return true;
}

BloomFilter::~BloomFilter() {
    delete[] filter;
}

unsigned long djb2(unsigned char *data) {
    int c;
    unsigned long hash = 5381;
    while ((c = *data++)){
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

unsigned long sdbm(unsigned char *data) {
    int c;
    unsigned long hash = 0;
    while ((c = *data++)){
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

unsigned long hash_i(unsigned char *data, unsigned int i) {
    return djb2(data) + i * sdbm(data) + i * i;
}

unsigned char* int_to_charptr(int id){
    string s1 = to_string(id);
    return (unsigned char *) s1.c_str();
}