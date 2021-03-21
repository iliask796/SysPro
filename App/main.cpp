#include <iostream>
#include "CitizenRecords.h"
using namespace std;

int main() {
    Record r1(4231,"Giannis Alpha","Greece",14);
    Record r2(2142,"Maria Beta","UK",16);
    Record r3(9214,"Alex Gamma","Iceland",13);
    Record r4(7563,"Eleni Delta","China",19);
    RecordTable rt(8);
    rt.insertElement(r1.getId(),&r1);
    rt.insertElement(r2.getId(),&r2);
    rt.insertElement(r3.getId(),&r3);
    rt.insertElement(r4.getId(),&r4);
    rt.displayTable();
    return 0;
}