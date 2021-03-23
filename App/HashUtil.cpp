#include "HashUtil.h"

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