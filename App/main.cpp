#include <iostream>
#include <fstream>
#include "BloomFilter.h"
#include "SkipList.h"
#include "InputHandler.h"
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
        currRecord = citizenData.getEntry(id);
        if (currRecord != NULL){
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
                date = "0-0-0000";
            }
        }
        if (counter == 7 and isVaccinated == "YES"){
            citizenFilters.addToFilter(virus,to_string(id));
            data = line.substr(start,end-start);
            date = data;
        }
        if (currRecord == NULL){
            currRecord = citizenData.insertElement(id,name,countries.getInfo(country),age);
        }
        //TODO Check if person already in lists before adding
        citizenVaccines.insert(virus,isVaccinated,currRecord,date);
    }
    cout << "*** Data Successfully Inserted ***" << endl;
    file.close();
    bool exit = false;
    CommandInput cmdi(9);
    string input1;
    string result;
    cout << "*** Type /help for available commands ***" << endl;
    while (!exit){
        cmdi.clear();
        cout << "*** Waiting For Action ***" << endl;
        getline(cin,input1);
        cmdi.insertCommand(input1);
        if (cmdi.getWord(0) == "/exit"){
            exit = true;
        }
        else if (cmdi.getWord(0) == "/help"){
            cout << "*** Listing available commands *** " << endl;
            cout << "/vaccineStatusBloom citizenID virusName" << endl << "/vaccineStatus citizenID virusName" << endl;
            cout << "/vaccineStatus citizenID" << endl << "/populationStatus [country] virusName date1 date2" << endl;
            cout << "/popStatusByAge [country] virusName date1 date2" << endl;
            cout << "/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]" << endl;
            cout << "/vaccinateNow citizenID firstName lastName country age virusName" << endl;
            cout << "/list-nonVaccinated-Persons virusName" << endl << "/exit" << endl;
            cout << "Caution: ID -> only numbers, date_format: 4-7-2020 and anything in [] -> optional" << endl;
        }
        else if (cmdi.getWord(0) == "/vaccineStatusBloom"){
            if (cmdi.getCount()!=3){
                cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                continue;
            }
            if ((cmdi.isDigit(1))==false){
                cout << "Error: Wrong ID format. Only numbers allowed." << endl;
                continue;
            }
            if (virusNames.getInfo(cmdi.getWord(2))==NULL){
                cout << "Error: Virus Not Found!" << endl;
                continue;
            }
            if (citizenFilters.probInFilter(cmdi.getWord(1),cmdi.getWord(2)) == 1){
                cout << "MAYBE" << endl;
            }
            else{
                cout << "NO" << endl;
            }
        }
        else if (cmdi.getWord(0) == "/vaccineStatus"){
            if ((cmdi.isDigit(1))==false){
                cout << "Error: Wrong ID format. Only numbers allowed." << endl;
                continue;
            }
            if (cmdi.getCount()==3){
                if (virusNames.getInfo(cmdi.getWord(2))==NULL){
                    cout << "Error: Virus Not Found!" << endl;
                    continue;
                }
                result = citizenVaccines.getVaccinateInfo(atoi(cmdi.getWord(1).c_str()),cmdi.getWord(2));
                if (result == "NO"){
                    cout << "NOT VACCINATED" << endl;
                }
                else if (result == "-1"){
                    cout << "Error: Relative info not found for this ID." << endl;
                }
                else{
                    cout << "VACCINATED ON " << result << endl;
                }
            }
            else if (cmdi.getCount()==2){
                if (!citizenVaccines.getVaccinateInfo(atoi(cmdi.getWord(1).c_str()))){
                    cout << "Error: Relative info not found for this ID." << endl;
                }
            }
            else{
                cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                continue;
            }
        }
        else if (cmdi.getWord(0) == "/list-nonVaccinated-Persons"){
            if (cmdi.getCount()!=2){
                cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                continue;
            }
            if (virusNames.getInfo(cmdi.getWord(1)) == NULL){
                cout << "Error: Virus Not Found!" << endl;
                continue;
            }
            citizenVaccines.displayNonVaccinated(cmdi.getWord(1));
        }
        else if (cmdi.getWord(0) == "/print"){
            citizenData.displayTable();
        }
        else if (cmdi.getWord(0) == "/test"){
            citizenVaccines.display();
        }
        cin.clear();
        fflush(stdin);
    }
    return 0;
}