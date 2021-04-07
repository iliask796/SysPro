#include <iostream>
#include <fstream>
#include "BloomFilter.h"
#include "SkipList.h"
using namespace std;

int main() {
//    SkipList sl(3);
//    sl.insert(12,"baba");
//    sl.insert(20,"baba");
//    sl.insert(4,"aaa");
//    sl.insert(17,"baba");
//    sl.insert(6,"baba");
//    sl.insert(26,"baba");
//    sl.insert(37,"baba");
//    sl.insert(1,"baba");
//    sl.insert(5,"baba");
//    sl.insert(11,"baba");
//    sl.insert(10,"baba");
//    sl.insert(9,"baba");
//    sl.display();
//    sl.remove(1);
//    sl.remove(4);
//    sl.remove(6);
//    sl.remove(9);
//    sl.remove(10);
//    sl.remove(17);
//    sl.remove(26);
//    sl.display();
//    cout << sl.getDate(4) << endl;
//    cout << sl.getDate(6) << endl;
//    cout << sl.getDate(2) << endl;
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
    VirusSkipList citizenVaccines(3);
    cout << "*** Reading Citizen Data ***" << endl;
    while(getline(file,line))
    {
        isVaccinated = "";
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
        if (counter == 6){
            data = line.substr(start,end-start);
            if (data == "NO"){
                isVaccinated = data;
            }
        }
        if (counter == 7 and isVaccinated == "YES"){
            citizenFilters.addToFilter(virus,to_string(id));
            data = line.substr(start,end-start);
            date = data;
        }
        currRecord = citizenData.insertElement(id,name,countries.getInfo(country),age);
        citizenVaccines.insert(virus,isVaccinated,currRecord,date);
    }
    cout << "*** Data Successfully Inserted ***" << endl;
    file.close();
    bool exit = false;
    string selection;
    string input1;
    string input2;
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
            if (citizenFilters.probInFilter(input1,input2) == 1){
                cout << "MAYBE" << endl;
            }
            else if (citizenFilters.probInFilter(input1,input2) == 0){
                cout << "NO" << endl;
            }
            else{
                cout << "Error: Virus Not Found!" << endl;
            }
        }
        else if (selection == "test"){
            citizenVaccines.display();
        }
        cin.clear();
        fflush(stdin);
    }
    return 0;
}