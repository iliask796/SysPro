#include <iostream>
#include <fstream>
#include "CitizenRecords.h"
using namespace std;

int main() {
    ifstream file;
    string filename = "D:\\SysPro\\Bash Script\\inputFile.txt";
    file.open(filename.c_str());
    string line;
    string data;
    int counter = 0;
    int id;
    bool duplicate;
    string name;
    int age;
    string country;
    string virus;
    string isVaccinated;
    string date;
    int start;
    int end;
    while(getline(file,line)){
        counter++;
    }
    file.close();
    file.open(filename.c_str());
    RecordTable rt(counter/2);
    while(getline(file,line))
    {
        duplicate = false;
        counter = 0;
        start = 0;
        end = line.find(' ');
        while (end != -1){
            data = line.substr(start,end-start);
            switch (counter) {
                case 0:
                    id = atoi(data.c_str());
                    if (rt.searchElement(id)){
                        duplicate = true;
                    }
                    break;
                case 1:
                    name = data;
                    break;
                case 2:
                    name += ' ' + data;
                    break;
                case 3:
                    country = data;
                    break;
                case 4:
                    age = atoi(data.c_str());
                    break;
                case 5:
                    virus = data;
                    break;
                case 6:
                    isVaccinated = data;
                    break;
            }
            if (duplicate){
                break;
            }
            start = end + 1;
            end = line.find(' ',start);
            counter++;
        }
        if (duplicate or isVaccinated == "NO"){
            cout << "Error in Record: " << line << endl;
            continue;
        }
        Record* r = new Record(id,name,country,age);
        rt.insertElement(r);
        if (counter == 7 and isVaccinated == "YES"){
            data = line.substr(start,end-start);
            date = data;
        }
    }
    rt.displayTable();
    file.close();
    return 0;
}