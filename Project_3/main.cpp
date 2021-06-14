#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h>
#include "CitizenRecords.h"
#include "BloomFilter.h"
#include "TravelRecords.h"
using namespace std;

#define PORT 6909
#define LOGFILE "/log_file."
#define FILEPERMS 0644

void catchCHLD(int);
void catchINT(int);
bool child_flag=false;
bool term_flag=false;
pid_t child2;

int main(int argc,char* argv[]){
    //Input checks and value initialization
    if (argc!=13){
        cout << "Error: Invalid Amount of App Arguments" << endl;
        return -1;
    }
    string arg1 = argv[1], arg3 = argv[3], arg5 = argv[5], arg7 = argv[7], arg9 = argv[9], arg11 = argv[11];
    if (arg1!="-m" or arg3!="-b" or arg5!="-c" or arg7!="-s" or arg9!="-i" or arg11!="-t"){
        cout << "Error: Invalid App Arguments" << endl;
        return -2;
    }
    int monitors= atoi(argv[2]);
    int buffSize = atoi(argv[4]);
    int cyclicSize = atoi(argv[6]);
    int bloomSize = atoi(argv[8]);
    int threads = atoi(argv[12]);
    string inputdir  = argv[10];
    int dirlen = inputdir.length();
    char dirname[dirlen];
    strcpy(dirname,inputdir.c_str());
    //Set signal handling
    static struct sigaction act1,act2;
    act1.sa_handler=catchCHLD;
    act2.sa_handler=catchINT;
    sigfillset(&(act1.sa_mask));
    sigfillset(&(act2.sa_mask));
    sigaction(SIGCHLD,&act1,NULL);
    sigaction(SIGINT,&act2,NULL);
    sigaction(SIGQUIT,&act2,NULL);
    //Start necessary actions for App
    char currentDirectory[400];
    int filedes;
    int i,j,k,pid[monitors],count,length,loc,SOCKET[monitors],port,retval=0,msgbuf2[buffSize];
    string country,virus;
    char msgbuf[buffSize+1];
    pid_t childpid;
    DIR* dir_ptr;
    struct dirent* direntp;
    struct sockaddr_in server;
    struct sockaddr* serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    InfoTable DirList(monitors);
    InfoList virusNames;
    virusNames.setParameters(0);
    BloomList* citizenFilter[monitors];
    int pos,neg;
    TravelDataTable posTravelData;
    TravelDataTable negTravelData;
    for (i=0;i<monitors;i++){
        citizenFilter[i] = new BloomList(bloomSize,3);
    }
    //Store folder names and divide them for each process
    if ((dir_ptr = opendir(dirname)) == NULL ) {
        cout << stderr << " cannot open " << dirname << endl;
    }
    else {
        i = 0;
        while ((direntp = readdir(dir_ptr)) != NULL) {
            if (direntp->d_name[0] != '.') {
                strcpy(msgbuf, dirname);
                strcat(msgbuf, "/");
                strcat(msgbuf, direntp->d_name);
                DirList.insertNode(i, msgbuf);
                if (++i == monitors) {
                    i = 0;
                }
            }
        }
        closedir(dir_ptr);
    }
    //Make a socket for each process
    for(i=0;i<monitors;i++){
        if ((SOCKET[i]=socket(AF_INET,SOCK_STREAM,0)) <0){
            perror("socket");
            exit(1);
        }
    }
    if((gethostname(msgbuf,buffSize))==1){
        perror("gethostname");
        exit(2);
    }
    if ((rem=gethostbyname(msgbuf)) == NULL){
        herror("gethostbyname");
        exit(2);
    }
    //Fork processes and exec monitor
    cout << "#Parent#" << " process ID: " << getpid() << ", parent ID: " << getppid() << endl;
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    const char* thread1 = to_string(threads).c_str();
    const char* buff1 = to_string(buffSize).c_str();
    const char* cyclic1 = to_string(cyclicSize).c_str();
    const char* bloom1 = to_string(bloomSize).c_str();
    for (i=0;i<monitors;i++){
        port = PORT+i;
        server.sin_port = htons(port);
        const char* port1 = to_string(port).c_str();
        childpid = fork();
        if (childpid == -1){
            perror("fork");
            exit(3);
        }
        if (childpid == 0){
            cout << "Child #" << i+1 << ", process ID: " << getpid() << ", parent ID: " << getppid() << endl;
            if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL){
                strcat(currentDirectory,"/Monitor");
            }
            char** arg = createArg(DirList.getList(i),port1,thread1,buff1,cyclic1,bloom1);
            retval = execvp(currentDirectory, arg);
            if(retval == -1) {
                perror("execvp");
                exit(4);
            }
            exit(0);
            delete[] arg;
        }
        pid[i]=childpid;
        sleep(3);
        if (connect(SOCKET[i],serverptr,sizeof(server)) < 0){
            perror("connect");
            exit(4);
        }
    }
    sleep(2);
    //Read filters
    for (i=0;i<monitors;i++){
        if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
            perror(" problem in reading ");
            exit(6);
        }
        count=*msgbuf;
        for (j=0;j<count;j++){
            if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
                perror(" problem in reading ");
                exit(6);
            }
            length=*msgbuf;
            if (read(SOCKET[i],msgbuf,length) < 0) {
                perror(" problem in reading ");
                exit(6);
            }
            msgbuf[length]='\0';
            virus.assign(msgbuf);
            if (virusNames.getInfo(virus) == NULL) {
                virusNames.insertNode(virus);
                virusNames.increment();
            }
            int* filter = new int[bloomSize/sizeof(int)];
            length=0;
            loc=buffSize/sizeof(int);
            while(length<bloomSize/buffSize){
                if (read(SOCKET[i],msgbuf2,buffSize) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                for (k=0;k<loc;k++){
                    filter[k+length*loc]=(msgbuf2[k]);
                }
                length++;
                if(length==bloomSize/buffSize and bloomSize%buffSize>0){
                    if (read(SOCKET[i],msgbuf2,bloomSize%buffSize) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    for (k=0;k<loc;k++){
                        filter[k+length*loc]=(msgbuf2[k]);
                        if(k==(bloomSize%buffSize)/sizeof(int)){
                            break;
                        }
                    }
                }
            }
            citizenFilter[i]->addFilter(virus,filter);
        }
    }
    int ready;
    int progress=0;
    //Get ready check from every process
    for (int c=0;c<monitors;c++){
        if (read(SOCKET[c],&ready,sizeof(int)) < 0) {
            perror(" problem in reading ");
            exit(6);
        }
        if (ready==1){
            progress++;
        }
    }
    if (progress == monitors){
        //If all good then get ready for input requests
        bool quit = false;
        CommandInput cmdi(9);
        int data,idToCheck,choice;
        string input1,result;
        cout << "*** Type /help for available commands ***" << endl;
        while (!quit) {
            cmdi.clear();
            if (term_flag){
                if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL){
                    strcat(currentDirectory,LOGFILE);
                    strcat(currentDirectory,to_string(getpid()).c_str());
                }
                if ((filedes=creat(currentDirectory,FILEPERMS))==-1){
                    perror ("creating") ;
                    exit(8);
                }
                for(i=0; i < monitors; i++){
                    delete citizenFilter[i];
                    kill(pid[i],SIGKILL);
                    sleep(1);
                    for(j=1;j<=DirList.getCapacity(i);j++){
                        write(filedes,DirList.getEntry(i,j).c_str(),DirList.getEntry(i,j).length());
                        write(filedes,"\n",1);
                    }
                }
                pos = posTravelData.getTotalRequests();
                neg = negTravelData.getTotalRequests();
                write(filedes,"TOTAL TRAVEL REQUESTS ",22);
                write(filedes,to_string(pos+neg).c_str(),to_string(pos+neg).length());
                write(filedes,"\n",1);
                write(filedes,"ACCEPTED ",9);
                write(filedes,to_string(pos).c_str(),to_string(pos).length());
                write(filedes,"\n",1);
                write(filedes,"REJECTED ",9);
                write(filedes,to_string(neg).c_str(),to_string(neg).length());
                write(filedes,"\n",1);
                return -3;
            }
            if (child_flag){
                for(i=0;i<monitors;i++){
                    if (pid[i]==child2){
                        cout << "Forking Process " << child2 << " again." << endl;
                        childpid = fork();
                        if (childpid == -1){
                            perror("fork");
                            exit(3);
                        }
                        if (childpid == 0){
                            cout << "Child #" << i+1 << ", process ID: " << getpid() << ", parent ID: " << getppid() << endl;
                            if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL){
                                strcat(currentDirectory,"/Monitor");
                            }
                            retval = execlp(currentDirectory, to_string(port).c_str(), NULL);
                            if(retval == -1) {
                                perror("execl");
                                exit(4);
                            }
                            exit(0);
                        }
                        pid[i]=childpid;
                        sleep(1);
                        if ((write(SOCKET[i],&buffSize,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        if ((write(SOCKET[i],&bloomSize,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        count = DirList.getCapacity(i);
                        if ((write(SOCKET[i],dirname,buffSize)) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        if ((write(SOCKET[i],&count,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        for (j=1;j<=count;j++){
                            country = DirList.getEntry(i,j);
                            length=country.size();
                            if ((write(SOCKET[i],&length,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            if ((write(SOCKET[i],country.c_str(),length)) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                        }
                        sleep(2);
                        if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
                            perror(" problem in reading ");
                            exit(6);
                        }
                        count=*msgbuf;
                        citizenFilter[i]->deleteFilter();
                        for (j=0;j<count;j++){
                            if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
                                perror(" problem in reading ");
                                exit(6);
                            }
                            length=*msgbuf;
                            if (read(SOCKET[i],msgbuf,length) < 0) {
                                perror(" problem in reading ");
                                exit(6);
                            }
                            msgbuf[length]='\0';
                            virus.assign(msgbuf);
                            if (virusNames.getInfo(virus) == NULL) {
                                virusNames.insertNode(virus);
                                virusNames.increment();
                            }
                            int* filter = new int[bloomSize/sizeof(int)];
                            length=0;
                            loc=buffSize/sizeof(int);
                            while(length<bloomSize/buffSize){
                                if (read(SOCKET[i],msgbuf2,buffSize) < 0) {
                                    perror(" problem in reading ");
                                    exit(6);
                                }
                                for (k=0;k<loc;k++){
                                    filter[k+length*loc]=(msgbuf2[k]);
                                }
                                length++;
                                if(length==bloomSize/buffSize and bloomSize%buffSize>0){
                                    if (read(SOCKET[i],msgbuf2,bloomSize%buffSize) < 0) {
                                        perror(" problem in reading ");
                                        exit(6);
                                    }
                                    for (k=0;k<loc;k++){
                                        filter[k+length*loc]=(msgbuf2[k]);
                                        if(k==(bloomSize%buffSize)/sizeof(int)){
                                            break;
                                        }
                                    }
                                }
                            }
                            citizenFilter[i]->addFilter(virus,filter);
                        }
                        if (read(SOCKET[i],&ready,sizeof(int)) < 0) {
                            perror(" problem in reading ");
                            exit(6);
                        }
                        if (ready==1){
                            cout << "Forking was successful." << endl;
                        }
                        else {
                            cout << "Forking has failed." << endl;
                        }
                    }
                }
                child_flag=false;
            }
            cout << "*** Waiting For Action ***" << endl;
            getline(cin, input1);
            cmdi.insertCommand(input1);
            if (cmdi.getWord(0) == "/exit") {
                quit = true;
                term_flag = true;
            }
            else if (cmdi.getWord(0) == "/help") {
                cout << "*** Listing available commands *** " << endl;
                cout << "/travelRequest citizenID date countryFrom countryTo virusName" << endl;
                cout << "/travelStats virusName date1 date2 [country]" << endl << "/addVaccinationRecords country" << endl;
                cout << "/searchVaccinationStatus citizenID" << endl;
                cout << "/quit" << endl;
                cout << "Caution: ID -> only numbers, date_format: 4-7-2020 and anything in [] -> optional" << endl;
            }
            else if (cmdi.getWord(0) == "/travelRequest"){
                if (cmdi.getCount()!=6){
                    cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                    continue;
                }
                if (!(cmdi.isDigit(1))){
                    cout << "Error: Wrong ID format. Only numbers allowed." << endl;
                    continue;
                }
                if (!cmdi.isDate(2)){
                    cout << "Error: Wrong DATE format. DD-MM-YYYY allowed." << endl;
                    continue;
                }
                if (virusNames.getInfo(cmdi.getWord(5))==NULL){
                    cout << "Error: Virus Not Found." << endl;
                    continue;
                }
                data=DirList.getInfo(inputdir+"/"+cmdi.getWord(3));
                if (data==-1){
                    cout << "Error: Country Not Found." << endl;
                    continue;
                }
                else{
                    cout << "Country handled by process: " << pid[data] << endl;
                }
                if (citizenFilter[data]->probInFilter(cmdi.getWord(1),cmdi.getWord(5)) == 1){
                    kill(pid[data],SIGUSR2);
                    idToCheck=atoi(cmdi.getWord(1).c_str());
                    choice=0;
                    if ((write(SOCKET[data],&choice,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(SOCKET[data],&idToCheck,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    length=cmdi.getWord(5).length();
                    if ((write(SOCKET[data],&length,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(SOCKET[data],cmdi.getWord(5).c_str(),length)) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if (read(SOCKET[data],msgbuf,sizeof(int)) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    length=*msgbuf;
                    if (read(SOCKET[data],msgbuf,length) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    msgbuf[length]='\0';
                    if (msgbuf == "NO"){
                        cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED" << endl;
                        result = "NO";
                        length = result.length();
                        if ((write(SOCKET[data],&length,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        if ((write(SOCKET[data],result.c_str(),length)) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                    }
                    else{
                        if (safeDateCheck(msgbuf,cmdi.getWord(2))==0){
                            cout << "REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE" << endl;
                            negTravelData.insertNode(cmdi.getWord(5),DirList.getCountryInfo(cmdi.getWord(3)),cmdi.getWord(2));
                            result = "NO";
                            length = result.length();
                            if ((write(SOCKET[data],&length,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            if ((write(SOCKET[data],result.c_str(),length)) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                        }
                        else{
                            cout << "REQUEST ACCEPTED – HAPPY TRAVELS" << endl;
                            posTravelData.insertNode(cmdi.getWord(5),DirList.getCountryInfo(cmdi.getWord(3)),cmdi.getWord(2));
                            result = "YES";
                            length = result.length();
                            if ((write(SOCKET[data],&length,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            if ((write(SOCKET[data],result.c_str(),length)) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                        }
                    }
                }
                else{
                    cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED" << endl;
                    negTravelData.insertNode(cmdi.getWord(5),DirList.getCountryInfo(cmdi.getWord(3)),cmdi.getWord(2));
                }
            }
            else if (cmdi.getWord(0) == "/travelStats"){
                if (virusNames.getInfo(cmdi.getWord(1))==NULL){
                    cout << "Error: Virus Not Found." << endl;
                    continue;
                }
                if (!cmdi.isDate(2) or !cmdi.isDate(3)){
                    cout << "Error: Wrong DATE format. DD-MM-YYYY allowed." << endl;
                    continue;
                }
                switch(cmdi.getCount()){
                    case 4:
                        cout << "Stats for every country." << endl;
                        pos = posTravelData.getRequests(cmdi.getWord(1),cmdi.getWord(2),cmdi.getWord(3));
                        neg = negTravelData.getRequests(cmdi.getWord(1),cmdi.getWord(2),cmdi.getWord(3));
                        cout << "TOTAL REQUESTS " << pos+neg << endl;
                        cout << "ACCEPTED " << pos << endl;
                        cout << "REJECTED " << neg << endl;
                        break;
                    case 5:
                        data=DirList.getInfo(inputdir+"/"+cmdi.getWord(4));
                        if (data==-1){
                            cout << "Error: Country Not Found." << endl;
                            break;
                        }
                        cout << "Stats for one country." << endl;
                        pos = posTravelData.getRequests(cmdi.getWord(1),cmdi.getWord(4),cmdi.getWord(2),cmdi.getWord(3));
                        neg = negTravelData.getRequests(cmdi.getWord(1),cmdi.getWord(4),cmdi.getWord(2),cmdi.getWord(3));
                        cout << "TOTAL REQUESTS " << pos+neg << endl;
                        cout << "ACCEPTED " << pos << endl;
                        cout << "REJECTED " << neg << endl;
                        break;
                    default:
                        cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                        break;
                }
            }
            else if (cmdi.getWord(0) == "/addVaccinationRecords"){
                if (cmdi.getCount()!=2){
                    cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                    continue;
                }
                data=DirList.getInfo(inputdir+"/"+cmdi.getWord(1));
                if (data==-1){
                    cout << "Error: Country Not Found." << endl;
                    continue;
                }
                else{
                    cout << "Country handled by process: " << pid[data] << endl;
                    kill(pid[data],SIGUSR1);
                    sleep(1);
                    length=cmdi.getWord(1).length();
                    if ((write(SOCKET[data],&length,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(SOCKET[data],cmdi.getWord(1).c_str(),length)) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    sleep(1);
                    //TODO: ADD RECORDS
                }
            }
            else if (cmdi.getWord(0) == "/searchVaccinationStatus"){
                if (cmdi.getCount()!=2){
                    cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                    continue;
                }
                if (!(cmdi.isDigit(1))){
                    cout << "Error: Wrong ID format. Only numbers allowed." << endl;
                    continue;
                }
                for(i=0;i<monitors;i++){
                    kill(pid[i],SIGUSR2);
                    idToCheck=atoi(cmdi.getWord(1).c_str());
                    choice=1;
                    if ((write(SOCKET[i],&choice,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(SOCKET[i],&idToCheck,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    sleep(1);
                    if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    choice=*msgbuf;
                    if (choice==1){
                        if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
                            perror(" problem in reading ");
                            exit(6);
                        }
                        choice=*msgbuf;
                        if (choice==1){
                            if (read(SOCKET[i],msgbuf,sizeof(int)) < 0) {
                                perror(" problem in reading ");
                                exit(6);
                            }
                            length=*msgbuf;
                            if (read(SOCKET[i],msgbuf,length) < 0) {
                                perror(" problem in reading ");
                                exit(6);
                            }
                            msgbuf[length]='\0';
                            cout << msgbuf;
                        }
                    }
                }
            }
            cin.clear();
            fflush(stdin);
        }
    }
    else {
        cout << "Error: Process initial data transfer was not successful." << endl;
    }
    for (i=0;i<monitors;i++){
        close(SOCKET[i]);
    }
    cout << "Exiting." << endl;
    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL){
        strcat(currentDirectory,LOGFILE);
        strcat(currentDirectory,to_string(getpid()).c_str());
    }
    if ((filedes=creat(currentDirectory,FILEPERMS))==-1){
        perror ("creating") ;
        exit(8);
    }
    for(i=0;i<monitors;i++){
        delete citizenFilter[i];
        kill(pid[i],SIGKILL);
        sleep(1);
        for(j=1;j<=DirList.getCapacity(i);j++){
            write(filedes,DirList.getEntry(i,j).c_str(),DirList.getEntry(i,j).length());
            write(filedes,"\n",1);
        }
    }
    pos = posTravelData.getTotalRequests();
    neg = negTravelData.getTotalRequests();
    write(filedes,"TOTAL TRAVEL REQUESTS ",22);
    write(filedes,to_string(pos+neg).c_str(),to_string(pos+neg).length());
    write(filedes,"\n",1);
    write(filedes,"ACCEPTED ",9);
    write(filedes,to_string(pos).c_str(),to_string(pos).length());
    write(filedes,"\n",1);
    write(filedes,"REJECTED ",9);
    write(filedes,to_string(neg).c_str(),to_string(neg).length());
    write(filedes,"\n",1);
    return 0;
}

void catchCHLD(int signo){
    if (!term_flag){
        cout << "@M CHLD CAUGHT with: " << signo  << endl;
        child2 = wait(NULL);
        child_flag=true;
    }
}

void catchINT(int signo){
    cout << "@M INT/QUIT CAUGHT with: " << signo << endl;
    term_flag=true;
}