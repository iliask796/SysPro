#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include "CitizenRecords.h"
#include "BloomFilter.h"
using namespace std;

#define RECORDTABLESIZE 100
#define BUFFSIZE 500

int main(int argc, char *argv[]){
    int fd,entriesNum,i,length,start,end,counter,id,age;
    char msgbuf[BUFFSIZE+1];
    string inputdir,subdir,line,filePath,name,country,virus,isVaccinated,date,data;
    ifstream countryFile;
    DIR* dir_ptr;
    struct dirent* direntp;
    cout << "Hello World" << endl;

    Record* currRecord;
    bool faultyRecord;
    InfoList countries;
    countries.setParameters(0);
    RecordTable citizenData(RECORDTABLESIZE);
    int bloomSize = 100000;
    BloomList citizenFilters(bloomSize,3);
//    VirusSkipList citizenVaccines(3);

    sleep(2);
    if ((fd= open(argv[0], O_RDWR)) < 0) {
        perror(" fifo open problem ");
        exit(3);
    }
    if (read(fd,msgbuf,BUFFSIZE) < 0) {
        perror(" problem in reading ");
        exit(5);
    }
    inputdir.assign(msgbuf);
    if (read(fd,msgbuf,sizeof(int)) < 0) {
        perror(" problem in reading ");
        exit(5);
    }
    cout << "This is process: " << getpid() << endl;
    entriesNum = *msgbuf;
    for (i=1;i<=entriesNum;i++){
        if (read(fd, msgbuf, sizeof(int)) < 0) {
            perror(" problem in reading ");
            exit(5);
        }
        length=*msgbuf;
        if (read(fd,msgbuf,length) < 0) {
            perror(" problem in reading ");
            exit(5);
        }
        msgbuf[length]='\0';
        subdir.assign(msgbuf);
        subdir = inputdir + "/" + subdir;
        if ((dir_ptr = opendir(subdir.c_str())) == NULL) {
            cout << stderr << " cannot open " << subdir << endl;
        }
        else {
//            cout << "Directory " << msgbuf << " successfully opened." << endl;
            while ((direntp = readdir(dir_ptr)) != NULL ){
                if (direntp->d_name[0] != '.'){
//                    cout << "File " << direntp->d_name << " found here." << endl;
                    filePath = subdir + "/" + direntp->d_name;
                    countryFile.open(filePath.c_str());
//                    cout << "Reading file." << endl;
                    while(getline(countryFile,line)){
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
//                                    if (virusNames.getInfo(virus)==NULL){
//                                        virusNames.insertNode(virus);
//                                    }
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
//                            cout << "Error in Record: " << line << endl;
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
                            citizenData.insertElement(id,name,countries.getInfo(country),age);
//                            countries.increasePopulation(country);
                        }
//                        if (citizenVaccines.getVaccinateInfo(currRecord->getId(),virus) == "-1"){
//                            citizenVaccines.insert(virus, isVaccinated, currRecord, date);
//                        }
//                        else {
//                            cout << "Error in Record: " << line << endl;
//                        }
                    }
//                    cout << "Reached the end of file." << endl;
                    countryFile.close();
                }
            }
//            cout << "Closing Directory." << endl;
            closedir(dir_ptr);
        }
    }
	return 0;
}
