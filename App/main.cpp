#include <iostream>
#include <fstream>
#include "CitizenRecords.h"
#include "BloomFilter.h"
using namespace std;

int main() {
    ifstream file;
    string filename = "D:\\SysPro\\Bash Script\\inputFile.txt";
    file.open(filename.c_str());
    string line;
    string data;
    int start;
    int end;
    int counter = 0;
    int id;
    string name;
    int age;
    string country;
    string virus;
    string isVaccinated;
    string date;
    Record* currRecord;
    bool faultyRecord;
    while(getline(file,line)){
        counter++;
    }
    file.close();
    file.open(filename.c_str());
    InfoList countries;
    InfoList virusNames;
    RecordTable citizenData(counter);
    BloomList citizenFilters(4000,3);
    cout << "*** Reading Citizen Data ***" << endl;
    while(getline(file,line))
    {
        faultyRecord = false;
        counter = 0;
        start = 0;
        end = line.find(' ');
        while (end != -1){
            data = line.substr(start,end-start);
            switch (counter) {
                case 0:
                    id = atoi(data.c_str());
                    break;
                case 1:
                    name = data;
                    break;
                case 2:
                    name += ' ' + data;
                    break;
                case 3:
                    country = data;
                    if (countries.getInfo(country)==NULL){
                        countries.insertNode(country);
                    }
                    break;
                case 4:
                    age = atoi(data.c_str());
                    break;
                case 5:
                    virus = data;
                    if (virusNames.getInfo(virus)==NULL){
                        virusNames.insertNode(virus);
                    }
                    break;
                case 6:
                    isVaccinated = data;
                    break;
            }
            start = end + 1;
            end = line.find(' ',start);
            counter++;
        }
        if (citizenData.searchElement(id)){
            currRecord = citizenData.getEntry(id);
            if (currRecord->getName() != name or currRecord->getCountry()->compare(country)!=0 or currRecord->getAge() != age){
                faultyRecord = true;
            }
        }
        if (isVaccinated == "NO" or faultyRecord){
            cout << "Error in Record: " << line << endl;
            continue;
        }
        if (counter == 7 and isVaccinated == "YES"){
            data = line.substr(start,end-start);
            date = data;
        }
        citizenData.insertElement(id,name,countries.getInfo(country),age);
        citizenFilters.addToFilter(virus,int_to_charptr(id));
    }
    cout << "*** Data Successfully Inserted ***" << endl;
    file.close();
    bool exit = false;
    string selection;
    string input1;
    int input2;
    while (!exit){
        cout << "*** Waiting For Action ***" << endl;
        cin >> selection;
        if (selection == "/exit"){
            exit = true;
        }
        else if (selection == "/print"){
            citizenData.displayTable();
        }
        else if (selection == "/vaccineStatusBloom"){
            cin >> input1;
            cin >> input2;
            if (citizenFilters.probInFilter(input1,int_to_charptr(input2))==1){
                cout << "MAYBE" << endl;
            }
            else if (citizenFilters.probInFilter(input1,int_to_charptr(input2))==0){
                cout << "NO" << endl;
            }
            else{
                cout << "Error: Virus Not Found!" << endl;
            }
        }
        cin.clear();
        fflush(stdin);
    }
    return 0;
}