#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include "BloomFilter.h"
#include "SkipList.h"
using namespace std;

#define RECORDTABLESIZE 100
#define LOGFILE "/log_file."
#define FILEPERMS 0644

int sig_flag=0;
void catchINT(int);
void catchUSR1(int);
void catchUSR2(int);

int main(int argc, char *argv[]){
    //Set signal handling
    static struct sigaction act1,act2,act3;
    act1.sa_handler=catchINT;
    act2.sa_handler=catchUSR1;
    act3.sa_handler=catchUSR2;
    sigfillset(&(act1.sa_mask));
    sigfillset(&(act2.sa_mask));
    sigfillset(&(act3.sa_mask));
    sigaction(SIGINT,&act1,NULL);
    sigaction(SIGQUIT,&act1,NULL);
    sigaction(SIGUSR1,&act2,NULL);
    sigaction(SIGUSR2,&act3,NULL);
    //Start necessary actions for Monitor
    int port,sock,newsock,filedes,i,j,length,start,end,counter,id,age,loc,info[1],pos=0,neg=0;
    string line,filePath,name,country,virus,isVaccinated,date,data;
    char currentDirectory[400];
    int* filter;
    ifstream countryFile;
    DIR* dir_ptr;
    struct dirent* direntp;
    struct sockaddr_in server,client;
    socklen_t clientlen;
    struct sockaddr *serverptr =(struct sockaddr*)&server;
    struct sockaddr *clientptr =(struct sockaddr*)&client;
    struct hostent *rem;
    Record* currRecord;
    bool faultyRecord;
    InfoList countries;
    countries.setParameters(0);
    InfoList virusNames;
    virusNames.setParameters(0);
    RecordTable citizenData(RECORDTABLESIZE);
    VirusSkipList citizenVaccines(3);
    sleep(2);
    //Open socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket");
        exit(1);
    }
    //Bind socket
    if((gethostname(currentDirectory,399))==1){
        perror("gethostname");
        exit(2);
    }
    if ((rem=gethostbyname(currentDirectory)) == NULL){
        herror("gethostbyname");
        exit(2);
    }
    server.sin_family= AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    port = atoi(argv[1]);
    server.sin_port = htons(port);
    if (bind(sock,serverptr,sizeof(server))<0){
        perror("bind");
        exit(3);
    }
    //Listen for connection
    if (listen(sock,5)<0){
        perror("listen");
        exit(4);
    }
    cout << getpid() << ": Listening for connection to port " << port << endl;
    //Accept connection
    if ((newsock=accept(sock,clientptr,&clientlen)) <0){
        perror("accept");
        exit(5);
    }
    cout << "Accepted connection." << endl;
    //Set parameters given from main program
    int folders = argc-10;
    string folderNames[folders];
    j=0;
    for (i=10;i<argc;i++){
        folderNames[j++].assign(argv[i]);
    }
    int threads = atoi(argv[3]);
    int buffSize = atoi(argv[5]);
    char msgbuf[buffSize+1];
    int cyclicSize = atoi(argv[7]);
    int bloomSize = atoi(argv[9]);
    BloomList citizenFilters(bloomSize,3);
    int dirCount[folders];
    for (i=1;i<=folders;i++) {
        dirCount[i-1]=0;
        //Open the subdirectory
        if ((dir_ptr = opendir(folderNames[i-1].c_str())) == NULL) {
            cout << stderr << " cannot open " << folderNames[i-1] << endl;
        } else {
            //Access each file and initiate data structures
            while ((direntp = readdir(dir_ptr)) != NULL) {
                if (direntp->d_name[0] != '.') {
                    filePath = folderNames[i-1] + "/" + direntp->d_name;
                    countryFile.open(filePath.c_str());
                    while(getline(countryFile,line))
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
                        if (citizenVaccines.getVaccinateInfo(currRecord->getId(),virus) == "-1"){
                            citizenVaccines.insert(virus, isVaccinated, currRecord, date);
                        }
                    }
                    countryFile.close();
                }
                dirCount[i-1]+=1;
            }
            closedir(dir_ptr);
        }
    }
    length=virusNames.getCapacity();
    //Send amount of viruses found
    if ((write(newsock,&length,sizeof(int))) == -1) {
        perror("Error in Writing");
        exit(5);
    }
    //Send each filter for every virus
    for (j=0;j<length;j++){
        virus = virusNames.getEntry(j+1);
        counter = virus.length();
        if ((write(newsock,&counter,sizeof(int))) == -1) {
            perror("Error in Writing");
            exit(5);
        }
        if ((write(newsock,virus.c_str(),counter)) == -1) {
            perror("Error in Writing");
            exit(5);
        }
        filter=citizenFilters.getFilter(virus);
        counter=0;
        loc= buffSize/sizeof(int);
        while(counter< bloomSize/buffSize){
            if ((write(newsock,&filter[counter*loc],buffSize)) == -1) {
                perror("Error in Writing");
                exit(5);
            }
            counter++;
            if(counter== bloomSize/buffSize and bloomSize%buffSize > 0){
                if ((write(newsock,&filter[counter*loc],bloomSize % buffSize)) == -1) {
                    perror("Error in Writing");
                    exit(5);
                }
            }
        }
    }
    //Send ready state message
    int ready=1;
    if ((write(newsock,&ready,sizeof(int))) == -1) {
        perror("Error in Writing");
        exit(5);
    }
    //Wait until something happens
    bool alive=true;
    int choice,found;
    string answer,part1,part2;
    while(alive){
        if (sig_flag==0){
            pause();
        }
        switch (sig_flag) {
            case 1:
                sig_flag=0;
                i=countries.getCapacity();
                if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL){
                    strcat(currentDirectory,LOGFILE);
                    strcat(currentDirectory,to_string(getpid()).c_str());
                }
                if ((filedes=creat(currentDirectory,FILEPERMS))==-1){
                    perror ("creating") ;
                    exit(8);
                }
                for (j=0;j<i;j++){
                    write(filedes,countries.getEntry(j+1).c_str(),countries.getEntry(j+1).length());
                    write(filedes,"\n",1);
                }
                write(filedes,"TOTAL TRAVEL REQUESTS ",22);
                write(filedes,to_string(pos+neg).c_str(),to_string(pos+neg).length());
                write(filedes,"\n",1);
                write(filedes,"ACCEPTED ",9);
                write(filedes,to_string(pos).c_str(),to_string(pos).length());
                write(filedes,"\n",1);
                write(filedes,"REJECTED ",9);
                write(filedes,to_string(neg).c_str(),to_string(neg).length());
                write(filedes,"\n",1);
                alive=false;
                break;
            case 2:
                sig_flag=0;
                cout << "New File Notification." << endl;
                if (read(newsock, msgbuf, sizeof(int)) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                length = *msgbuf;
                if (read(newsock,msgbuf,length) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                msgbuf[length] = '\0';
                answer.assign(msgbuf);
                countries.getCapacity();
                for (i=1;i<=countries.getCapacity();i++){
                    if (countries.getEntry(i)==answer){
                        break;
                    }
                }
                found=0;
                for(i=0;i<folders;i++){
                    if (folderNames[i].find(answer) != std::string::npos) {
                        if ((dir_ptr = opendir(folderNames[i].c_str())) == NULL) {
                            cout << stderr << " cannot open " << folderNames[i] << endl;
                        }
                        else {
                            while ((direntp = readdir(dir_ptr)) != NULL) {
                                found++;
                            }
                        }
                    }
                }
                cout << "Found " << found-dirCount[i-1] << " new file(s)." << endl;
                break;
            case 3:
                sig_flag=0;
                if (read(newsock,info,sizeof(int)) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                choice = *info;
                if (read(newsock,info,sizeof(int)) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                id = *info;
                if (choice==0){
                    if (read(newsock,info,sizeof(int)) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    if (read(newsock,msgbuf,*info) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    msgbuf[*info]='\0';
                    virus.assign(msgbuf);
                    answer = citizenVaccines.getVaccinateInfo(id,virus);
                    length=answer.length();
                    if ((write(newsock,&length,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(newsock,answer.c_str(),length) == -1)) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if (read(newsock,info,sizeof(int)) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    if (read(newsock,msgbuf,*info) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    msgbuf[*info]='\0';
                    answer.assign(msgbuf);
                    if (answer == "NO"){
                        neg++;
                    }
                    else if (answer == "YES"){
                        pos++;
                    }
                }
                else if (choice==1){
                    currRecord=citizenData.getEntry(id);
                    if (currRecord!=NULL){
                        found=1;
                        if ((write(newsock,&found,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        part1 = to_string(id) + " " + currRecord->getName() + " " + *currRecord->getCountry() + "\n";
                        part1 += "AGE " + to_string(currRecord->getAge()) + "\n";
                        if ((part2=citizenVaccines.getVaccinateInfo(id))=="-1"){
                            found = 0;
                            if ((write(newsock,&found,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                        }
                        else{
                            if ((write(newsock,&found,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            answer = part1 + part2;
                            length=answer.length();
                            if ((write(newsock,&length,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            if ((write(newsock,answer.c_str(),length) == -1)) {
                                perror("Error in Writing");
                                exit(5);
                            }
                        }
                    }
                    else{
                        found=0;
                        if ((write(newsock,&found,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                    }
                }
                else{
                    cout << "Unknown action." << endl;
                }

                break;
            default:
                cout << "Caught a signal which is not being handled." << endl;
                break;
        }
    }
    cout << "Ending Process " << getpid() << "." << endl;
    close(sock);
    close(newsock);
	return 0;
}

void catchINT(int sig_no){
    cout << "@C INT/QUIT CAUGHT with: " << sig_no << endl;
    sig_flag = 1;
}

void catchUSR1(int sig_no){
    cout << "@C USR1 CAUGHT with: " << sig_no << endl;
    sig_flag = 2;
}

void catchUSR2(int sig_no){
    cout << "@C USR2 CAUGHT with: " << sig_no << endl;
    sig_flag = 3;
}