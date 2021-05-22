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
#include "TravelRecords.h"
using namespace std;

#define FIFONAME "my_fifo"
#define FIFONAMELEN 16
#define PERMS 0666
#define LOGFILE "/log_file."
#define FILEPERMS 0644

void catchCHLD(int);
void catchINT(int);
bool child_flag=false;
bool term_flag=false;
pid_t child2;

int main(int argc,char* argv[]){
    //Input checks and value initialization
    if (argc!=9){
        cout << "Error: Invalid Amount of App Arguments" << endl;
        return -1;
    }
    string arg1 = argv[1], arg3 = argv[3], arg5 = argv[5], arg7 = argv[7];
    if (arg1!="-m" or arg3!="-b" or arg5!="-s" or arg7!="-i"){
        cout << "Error: Invalid App Arguments" << endl;
        return -2;
    }
    int monitors= atoi(argv[2]);
    int buffSize = atoi(argv[4]);
    int bloomSize = atoi(argv[6]);
    string inputdir  = argv[8];
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
    int i,j,k,pid[monitors],count,length,loc,fd[monitors*2],retval=0,msgbuf2[buffSize];
    string country,virus;
    char msgbuf[buffSize+1],FIFO[monitors*2][FIFONAMELEN];
    pid_t childpid;
    DIR* dir_ptr;
    struct dirent* direntp;
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
    //Make and open pipes (2 for each process)
    for(i=0; i < monitors * 2; i++){
        strcpy(FIFO[i],(FIFONAME+to_string(i)).c_str());
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
    //Fork processes and exec monitor
    cout << "#Parent#" << " process ID: " << getpid() << ", parent ID: " << getppid() << endl;
    for (i=0;i<monitors;i++){
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
            retval = execlp(currentDirectory, FIFO[i], FIFO[i+monitors], NULL);
            if(retval == -1) {
                perror("execl");
                exit(4);
            }
            exit(0);
        }
        pid[i]=childpid;
        sleep(1);
    }
    //Store folder names and divide them for each process
    if ((dir_ptr = opendir(dirname)) == NULL ) {
        cout << stderr << " cannot open " << dirname << endl;
    }
    else {
        i=0;
        while ((direntp = readdir(dir_ptr)) != NULL ){
            if (direntp->d_name[0] != '.'){
                strcpy(msgbuf,direntp->d_name);
                DirList.insertNode(i,msgbuf);
                if (++i == monitors){
                    i=0;
                }
            }
        }
        closedir(dir_ptr);
        //Transfer initialization data
        for (i=0;i<monitors;i++){
            if ((write(fd[i],&buffSize,sizeof(int))) == -1) {
                perror("Error in Writing");
                exit(5);
            }
            if ((write(fd[i],&bloomSize,sizeof(int))) == -1) {
                perror("Error in Writing");
                exit(5);
            }
        }
        //Transfer directory names over pipes
        for (i=0;i<monitors;i++){
            count = DirList.getCapacity(i);
            if ((write(fd[i],dirname,buffSize)) == -1) {
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
        //Read filters
        for (i=0;i<monitors;i++){
            if (read(fd[i+monitors],msgbuf,sizeof(int)) < 0) {
                perror(" problem in reading ");
                exit(6);
            }
            count=*msgbuf;
            for (j=0;j<count;j++){
                if (read(fd[i+monitors],msgbuf,sizeof(int)) < 0) {
                    perror(" problem in reading ");
                    exit(6);
                }
                length=*msgbuf;
                if (read(fd[i+monitors],msgbuf,length) < 0) {
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
                    if (read(fd[i+monitors],msgbuf2,buffSize) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    for (k=0;k<loc;k++){
                        filter[k+length*loc]=(msgbuf2[k]);
                    }
                    length++;
                    if(length==bloomSize/buffSize and bloomSize%buffSize>0){
                        if (read(fd[i+monitors],msgbuf2,bloomSize%buffSize) < 0) {
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
    }
    int ready;
    int progress=0;
    //Get ready check from every process
    for (int c=0;c<monitors;c++){
        if (read(fd[c+monitors],&ready,sizeof(int)) < 0) {
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
                data=DirList.getInfo(cmdi.getWord(3));
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
                    if ((write(fd[data],&choice,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(fd[data],&idToCheck,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    length=cmdi.getWord(5).length();
                    if ((write(fd[data],&length,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(fd[data],cmdi.getWord(5).c_str(),length)) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if (read(fd[data+monitors],msgbuf,sizeof(int)) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    length=*msgbuf;
                    if (read(fd[data+monitors],msgbuf,length) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    msgbuf[length]='\0';
                    if (msgbuf == "NO"){
                        cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED" << endl;
                        result = "NO";
                        length = result.length();
                        if ((write(fd[data],&length,sizeof(int))) == -1) {
                            perror("Error in Writing");
                            exit(5);
                        }
                        if ((write(fd[data],result.c_str(),length)) == -1) {
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
                            if ((write(fd[data],&length,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            if ((write(fd[data],result.c_str(),length)) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                        }
                        else{
                            cout << "REQUEST ACCEPTED – HAPPY TRAVELS" << endl;
                            posTravelData.insertNode(cmdi.getWord(5),DirList.getCountryInfo(cmdi.getWord(3)),cmdi.getWord(2));
                            result = "YES";
                            length = result.length();
                            if ((write(fd[data],&length,sizeof(int))) == -1) {
                                perror("Error in Writing");
                                exit(5);
                            }
                            if ((write(fd[data],result.c_str(),length)) == -1) {
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
                        data=DirList.getInfo(cmdi.getWord(4));
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
                //TODO: ADD RECORDS
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
                    if ((write(fd[i],&choice,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    if ((write(fd[i],&idToCheck,sizeof(int))) == -1) {
                        perror("Error in Writing");
                        exit(5);
                    }
                    sleep(1);
                    if (read(fd[i+monitors],msgbuf,sizeof(int)) < 0) {
                        perror(" problem in reading ");
                        exit(6);
                    }
                    choice=*msgbuf;
                    if (choice==1){
                        if (read(fd[i+monitors],msgbuf,sizeof(int)) < 0) {
                            perror(" problem in reading ");
                            exit(6);
                        }
                        choice=*msgbuf;
                        if (choice==1){
                            if (read(fd[i+monitors],msgbuf,sizeof(int)) < 0) {
                                perror(" problem in reading ");
                                exit(6);
                            }
                            length=*msgbuf;
                            if (read(fd[i+monitors],msgbuf,length) < 0) {
                                perror(" problem in reading ");
                                exit(6);
                            }
                            msgbuf[length]='\0';
                            cout << msgbuf;
                        }
                    }
                }
            }
            else if (cmdi.getWord(0) == "/test") {
                cout << citizenFilter[0]->probInFilter("9058","Salivirus") << endl;
                cout << citizenFilter[0]->probInFilter("1885","Influenza-C") << endl;
                cout << citizenFilter[0]->probInFilter("1019","Mayaro") << endl;
                cout << citizenFilter[0]->probInFilter("5856","Dhori-0") << endl;
            }
            else if (cmdi.getWord(0) == "/test2"){
//                for (i=0;i<monitors;i++) {
//                    kill(pid[i],SIGINT);
//                }
                kill(pid[0],SIGINT);
                sleep(1);
            }
            else if (cmdi.getWord(0) == "/test3") {
                raise(SIGINT);
            }
            else if (cmdi.getWord(0) == "/test4"){
                cout << compareDates(cmdi.getWord(1),cmdi.getWord(2)) << endl;
            }
            cin.clear();
            fflush(stdin);
        }
    }
    else {
        cout << "Error: Process initial data transfer was not successful." << endl;
    }
    for (i=0;i<monitors*2;i++){
        close(fd[i]);
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
        cout << "@M CHLD CAUGHT with: " << signo  << " (";
        child2 = wait(NULL);
        child_flag=true;
        //TODO: FORK AGAIN
    }
}

void catchINT(int signo){
    cout << "@M INT/QUIT CAUGHT with: " << signo << endl;
    term_flag=true;
}

// ./App -m 3 -b 200 -s 1000 -i ../CountryLogs
// 1:USA,UK,Korea || 2:Germany,Japan || 3:Greece,Italy
// /travelRequest 123 11-11-1111 Greece countryTo H1N1
// /addVaccinationRecords USA
// /travelStats Salivirus date1 date2 [country]
// /travelStats Influenza-C date1 date2 [country]
// /travelStats Mayaro date1 date2
// /travelRequest 9058 11-11-1111 USA countryTo Salivirus (NO)
// /travelRequest 1885 11-11-1111 USA countryTo Influenza-C (MAYBE)
// /travelRequest 1019 11-11-1111 USA countryTo Mayaro (NO)
// /travelRequest 5856 11-11-1111 USA countryTo Dhori-0 (MAYBE)
// /searchVaccinationStatus 9058 (1)
// /searchVaccinationStatus 1019 (1)
// /searchVaccinationStatus 8825 (2)