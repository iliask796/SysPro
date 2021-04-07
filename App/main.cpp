#include <iostream>
#include <fstream>
#include "BloomFilter.h"
#include "SkipList.h"
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
                default:
                    break;
            }
            start = end + 1;
            end = line.find(' ',start);
            counter++;
        }
        if (citizenData.searchElement(id)){
            currRecord = citizenData.getEntry(id);
            if (currRecord->getName() != name or *currRecord->getCountry()!=country or currRecord->getAge() != age){
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
    cout << "*** Type /help for available commands ***" << endl;
    while (!exit){
        cout << "*** Waiting For Action ***" << endl;
        cin >> selection;
        if (selection == "/exit"){
            exit = true;
        }
        else if (selection == "/help"){
            cout << "*** Listing available commands *** " << endl;
            cout << "/vaccineStatusBloom citizenID virusName" << endl << "/vaccineStatus citizenID virusName" << endl;
            cout << "/vaccineStatus citizenID" << endl << "/populationStatus [country] virusName date1 date2" << endl;
            cout << "/popStatusByAge [country] virusName date1 date2" << endl;
            cout << "/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]" << endl;
            cout << "/vaccinateNow citizenID firstName lastName country age virusName" << endl;
            cout << "/list-nonVaccinated-Persons virusName" << endl << "/exit" << endl;
            cout << "Caution: ID -> only numbers, date_format: 4-7-2020 and anything in [] -> optional" << endl;
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
        else if (selection == "/print"){
            citizenData.displayTable();
        }
        else if (selection == "/test"){
            citizenVaccines.display();
            cout << "Test1: " << citizenVaccines.isNotVaccinated("H1N1",5660)<<endl;
            cout << "Test2: " << citizenVaccines.isNotVaccinated("H1N1",142)<<endl;
            cout << "Test3: " << citizenVaccines.isNotVaccinated("H1N11",5660)<<endl;
        }
        cin.clear();
        fflush(stdin);
    }
    return 0;
}