#include <string>
using namespace std;

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

unsigned long djb2(unsigned char*);
unsigned long sdbm(unsigned char*);
unsigned long hash_i(unsigned char*, unsigned int);
unsigned char* int_to_charptr(int);