#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include "CitizenRecords.h"
#include "BloomFilter.h"
#include "InputHandler.h"
using namespace std;

#define FIFONAMELEN 16
#define PERMS 0666

#define NUM 3
#define DIRLEN 20
#define TARGETDIR "../CountryLogs"
#define BUFFSIZE 200
#define BLOOMSIZE 1000

void catchCHLD(int);
void catchINT(int);
bool child_flag=false;
bool term_flag=false;
pid_t child2;

int main(){
    static struct sigaction act1,act2;
    act1.sa_handler=catchCHLD;
    act2.sa_handler=catchINT;
    sigfillset(&(act1.sa_mask));
    sigfillset(&(act2.sa_mask));
    sigaction(SIGCHLD,&act1,NULL);
    sigaction(SIGINT,&act2,NULL);
    sigaction(SIGQUIT,&act2,NULL);
    int i,j,k,pid[NUM],count,length,loc,fd[NUM*2],retval=0,msgbuf2[BUFFSIZE];
    string country,virus;
    char msgbuf[BUFFSIZE+1],dirname[DIRLEN] = TARGETDIR,FIFO[NUM*2][FIFONAMELEN];
    pid_t childpid;
    DIR* dir_ptr;
    struct dirent* direntp;
    InfoTable DirList(NUM);
    InfoList virusNames;
    virusNames.setParameters(0);
    int bloomSize = BLOOMSIZE;
    BloomList* citizenFilter[NUM];
    for (i=0;i<NUM;i++){
        citizenFilter[i] = new BloomList(bloomSize,3);
    }
    //make and open pipes (2 for each process)
    for(i=0;i<NUM*2;i++){
        strcpy(FIFO[i],("my_fifo"+to_string(i)).c_str());
        if ( mkfifo(FIFO[i],PERMS) == -1 ) {
            if (errno != EEXIST) {
                perror("receiver : mkfifo ");
                exit(1);
            }
        }
        if ((fd[i]= open(FIFO[i], O_RDWR)) < 0) {
            perror(" fifo open problem ");
            exit(2);
        }
    }
    //fork processes and exec monitor
    cout << "#Parent#" << " process ID: " << getpid() << ", parent ID: " << getppid() << endl;
    for (i=0;i<NUM;i++){
        childpid = fork();
        if (childpid == -1){
            perror("fork");
            exit(3);
        }
        if (childpid == 0){
            cout << "Child #" << i+1 << ", process ID: " << getpid() << ", parent ID: " << getppid() << endl;
            retval = execlp("/home/iliask/CLionProjects/SysPro/Project_2/App/cmake-build-debug/Monitor",FIFO[i],FIFO[i+NUM],NULL);
            if(retval == -1) {
                perror("execlp");
                exit(4);
            }
            exit(0);
        }
        pid[i]=childpid;
        sleep(1);
    }
    //store folder names and divide them for each process
    if ( (dir_ptr = opendir (dirname)) == NULL ) {
        cout << stderr << " cannot open " << dirname << endl;
    }
    else {
//        cout << "Directory " << dirname << " successfully opened." << endl;
        i=0;
        while ( (direntp = readdir(dir_ptr) ) != NULL ){
            if (direntp->d_name[0] != '.'){
//                cout << "Directory " << direntp->d_name << " found here." << endl;
                strcpy(msgbuf,direntp->d_name);
                DirList.insertNode(i,msgbuf);
                if (++i==NUM){
                    i=0;
                }
            }
        }
//        cout << "Closing Directory." << endl;
        closedir(dir_ptr);
        //transfer directory names over pipes
        for (i=0;i<NUM;i++){
            count = DirList.getCapacity(i);
            if ((write(fd[i],dirname,BUFFSIZE)) == -1) {
                perror("Error in Writing");
                exit(5);
            }
            if ((write(fd[i],&count,sizeof(int))) == -1) {
                perror("Error in Writing");
                exit(5);
            }
            for (j=1;j<=count;j++){
                country = DirList.getEntry(i,j);
                length=country.size();
                if ((write(fd[i],&length,sizeof(int))) == -1) {
                    perror("Error in Writing");
                    exit(5);
                }
                if ((write(fd[i],country.c_str(),length)) == -1) {
                    perror("Error in Writing");
                    exit(5);
                }
            }
        }
        sleep(2);
        cout << "This is the Parent Process" << endl;
        //read filters
        for (i=0;i<NUM;i++){
            if (read(fd[i+NUM],msgbuf,sizeof(int)) < 0) {
                perror(" problem in reading ");
                exit(6);
            }
            count=*msgbuf;
            for (j=0;j<count;j++){
                if (read(fd[i+NUM],msgbuf,sizeof(int)) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                length=*msgbuf;
                if (read(fd[i+NUM],msgbuf,length) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                msgbuf[length]='\0';
                virus.assign(msgbuf);
                if (virusNames.getInfo(virus) == NULL) {
                    virusNames.insertNode(virus);
                    virusNames.increment();
                }
                int* filter = new int[BLOOMSIZE/sizeof(int)];
                length=0;
                loc=BUFFSIZE/sizeof(int);
                while(length<BLOOMSIZE/BUFFSIZE){
                    if (read(fd[i+NUM],msgbuf2,BUFFSIZE) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    for (k=0;k<loc;k++){
                        filter[k+length*loc]=(msgbuf2[k]);
                    }
//                    cout << "Read Filter" << endl;
                    length++;
                    if(length==BLOOMSIZE/BUFFSIZE and BLOOMSIZE%BUFFSIZE>0){
                        if (read(fd[i+NUM],msgbuf2,BLOOMSIZE%BUFFSIZE) < 0) {
                            perror(" problem in reading ");
                            exit(6);
                        }
                        for (k=0;k<loc;k++){
                            filter[k+length*loc]=(msgbuf2[k]);
                            if(k==(BLOOMSIZE%BUFFSIZE)/sizeof(int)){
                                break;
                            }
                        }
                    }
                }
                citizenFilter[i]->addFilter(virus,filter);
            }
        }
    }
    int ready;
    int progress=0;
    //get ready from every process
    for (int c=0;c<NUM;c++){
        if (read(fd[c+NUM],&ready,sizeof(int)) < 0) {
            perror(" problem in reading ");
            exit(6);
        }
        if (ready==1){
            progress++;
        }
    }
    if (progress==NUM){
        bool quit = false;
        CommandInput cmdi(9);
        int data;
        string input1;
        cout << "*** Type /help for available commands ***" << endl;
        while (!quit) {
            cmdi.clear();
            if (term_flag){
                cout << "Killing the kids." << endl;
                cout << "These are the countries of the main process:" << endl;
                for(i=0;i<NUM;i++){
                    kill(pid[i],SIGKILL);
                    sleep(1);
                    for(j=1;j<=DirList.getCapacity(i);j++){
                        cout << DirList.getEntry(i,j) << endl;
                    }
                }
                return -1;
            }
            if (child_flag){
                for(i=0;i<NUM;i++){
                    if (pid[i]==child2){
                        cout << "Forking Process " << child2 << " again." << endl;
                    }
                }
                child_flag=false;
            }
            cout << "*** Waiting For Action ***" << endl;
            getline(cin, input1);
            cmdi.insertCommand(input1);
            if (cmdi.getWord(0) == "/quit") {
                quit = true;
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
                data=DirList.getInfo(cmdi.getWord(3));
                if (data==-1){
                    cout << "Error: Country Not Found." << endl;
                    continue;
                }
                else{
                    cout << "Country handled by process: " << pid[data] << endl;
                }
                cout << "TODO" << endl;
            }
            else if (cmdi.getWord(0) == "/addVaccinationRecords"){
                if (cmdi.getCount()!=2){
                    cout << "Error: Arguments Mismatch. Type /help for more info." << endl;
                    continue;
                }
                data=DirList.getInfo(cmdi.getWord(1));
                if (data==-1){
                    cout << "Error: Country Not Found." << endl;
                    continue;
                }
                else{
                    cout << "Country handled by process: " << pid[data] << endl;
                    kill(pid[data],SIGUSR1);
                    sleep(1);
                }
            }
            else if (cmdi.getWord(0) == "/test") {
                cout << citizenFilter[0]->probInFilter("9058","Salivirus") << endl;
                cout << citizenFilter[0]->probInFilter("1885","Influenza-C") << endl;
                cout << citizenFilter[0]->probInFilter("1019","Mayaro") << endl;
                cout << citizenFilter[0]->probInFilter("5856","Dhori-0") << endl;
            }
            else if (cmdi.getWord(0) == "/test2"){
//                for (i=0;i<NUM;i++) {
//                    kill(pid[i],SIGINT);
//                }
                kill(pid[0],SIGINT);
                sleep(1);
            }
            else if (cmdi.getWord(0) == "/test3") {
                raise(SIGINT);
            }
            cin.clear();
            fflush(stdin);
        }
    }
    else {
        cout << "Error: Process initial data transfer was not successful." << endl;
    }
    for (i=0;i<NUM*2;i++){
        close(fd[i]);
    }
    cout << "Killing the kids." << endl;
    cout << "These are the countries of the main process:" << endl;
    for(i=0;i<NUM;i++){
        kill(pid[i],SIGKILL);
        sleep(1);
        for(j=1;j<=DirList.getCapacity(i);j++){
            cout << DirList.getEntry(i,j) << endl;
        }
    }
    exit(0);
}

void catchCHLD(int signo){
    int status;
    cout << "@M CHLD CAUGHT with: " << signo  << " (";
    child2 = wait(&status);
    cout << child2 << "," << status << ")" << endl;
    child_flag=true;
}

void catchINT(int signo){
    cout << "@M INT/QUIT CAUGHT with: " << signo << endl;
    term_flag=true;
}

// /travelRequest 123 11-11-1111 Greece countryTo H1N1
// 1:USA,UK,Korea || 2:Germany,Japan || 3:Greece,Italy
// /addVaccinationRecords USA