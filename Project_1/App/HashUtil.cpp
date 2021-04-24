#include "HashUtil.h"

unsigned long djb2(string data) {
    int c;
    int i = 0;
    unsigned long hash = 5381;
    while ((c = data[i++])){
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

unsigned long sdbm(string data) {
    int c;
    int i = 0;
    unsigned long hash = 0;
    while ((c = data[i++])){
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

unsigned long hash_i(string data, unsigned int i) {
    return djb2(data) + i * sdbm(data) + i * i;
}