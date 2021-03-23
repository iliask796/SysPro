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