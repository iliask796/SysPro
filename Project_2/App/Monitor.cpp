#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "CitizenRecords.h"
#include "BloomFilter.h"
using namespace std;

#define RECORDTABLESIZE 100
#define BUFFSIZE 500
#define BLOOMSIZE 1000

int sig_flag;
void catchINT(int);
void catchUSR1(int);

int main(int argc, char *argv[]){
    static struct sigaction act1,act2;
    act1.sa_handler=catchINT;
    act2.sa_handler=catchUSR1;
    sigfillset(&(act1.sa_mask));
    sigfillset(&(act2.sa_mask));
    sigaction(SIGINT,&act1,NULL);
    sigaction(SIGQUIT,&act1,NULL);
    sigaction(SIGUSR1,&act2,NULL);
    int fd0,fd1,entriesNum,i,j,length,start,end,counter,id,age,loc;
    char msgbuf[BUFFSIZE+1];
    string inputdir,subdir,line,filePath,name,country,virus,isVaccinated,date,data;
    int* filter;
    ifstream countryFile;
    DIR* dir_ptr;
    struct dirent* direntp;
    cout << "Hello World" << endl;
    Record* currRecord;
    bool faultyRecord;
    InfoList countries;
    countries.setParameters(0);
    InfoList virusNames;
    virusNames.setParameters(0);
    RecordTable citizenData(RECORDTABLESIZE);
    int bloomSize = BLOOMSIZE;
    BloomList citizenFilters(bloomSize,3);
//    VirusSkipList citizenVaccines(3);
    sleep(2);
    if ((fd0= open(argv[0], O_RDWR)) < 0) {
        perror(" fifo open problem ");
        exit(3);
    }
    if ((fd1= open(argv[1], O_RDWR)) < 0) {
        perror(" fifo open problem ");
        exit(3);
    }
    if (read(fd0, msgbuf, BUFFSIZE) < 0) {
        perror(" problem in reading ");
        exit(5);
    }
    inputdir.assign(msgbuf);
    if (read(fd0, msgbuf, sizeof(int)) < 0) {
        perror(" problem in reading ");
        exit(5);
    }
    entriesNum = *msgbuf;
    cout << "This is process: " << getpid() << endl;
    for (i=1;i<=entriesNum;i++){
        if (read(fd0, msgbuf, sizeof(int)) < 0) {
            perror(" problem in reading ");
            exit(5);
        }
        length=*msgbuf;
        if (read(fd0, msgbuf, length) < 0) {
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
                                        countries.increment();
                                    }
                                    break;
                                case 4:
                                    age = atoi(data.c_str());
                                    break;
                                case 5:
                                    virus = data;
                                    if (virusNames.getInfo(virus)==NULL){
                                        virusNames.insertNode(virus);
                                        virusNames.increment();
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
            length=virusNames.getCapacity();
            if ((write(fd1, &length, sizeof(int))) == -1) {
                perror("Error in Writing");
                exit(5);
            }
            for (j=0;j<length;j++){
                virus = virusNames.getEntry(j+1);
                counter = virus.length();
                if ((write(fd1,&counter,sizeof(int))) == -1) {
                    perror("Error in Writing");
                    exit(5);
                }
                if ((write(fd1,virus.c_str(),counter)) == -1) {
                    perror("Error in Writing");
                    exit(5);
                }
                filter=citizenFilters.getFilter(virus);
                counter=0;
                loc=BUFFSIZE/sizeof(int);
                while(counter<BLOOMSIZE/BUFFSIZE){
                    if ((write(fd1,&filter[counter*loc],BUFFSIZE)) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    counter++;
                }
            }
        }
    }
//    while(true){
//        pause();
//        switch (sig_flag) {
//            case 1:
//                i=countries.getCapacity();
//                for (j=0;j<i;j++){
//                    cout << countries.getEntry(j+1) << endl;
//                }
//                break;
//            case 2:
//                cout << "New File Notification. Make new Bloom Filters." << endl;
//                break;
//            default:
//                break;
//        }
//    }
    pause();
    cout << "Ending" << endl;
    close(fd0);
    close(fd1);
	return 0;
}

void catchINT(int sig_no){
    cout << "CAUGHT with: " << sig_no << endl;
    sig_flag = 1;
}

void catchUSR1(int sig_no){
    cout << "CAUGHT with: " << sig_no << endl;
    sig_flag = 2;
}