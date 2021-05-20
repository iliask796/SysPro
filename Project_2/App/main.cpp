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
using namespace std;

#define FIFONAMELEN 16
#define PERMS 0666

#define NUM 3
#define DIRLEN 20
#define TARGETDIR "../CountryLogs"
#define BUFFSIZE 500

int main(){
    int i,j,count,length,fd[NUM],retval=0;
    string country;
    char msgbuf[BUFFSIZE+1],dirname[DIRLEN] = TARGETDIR,FIFO[NUM][FIFONAMELEN];
    pid_t childpid;
    DIR* dir_ptr;
    struct dirent* direntp;
    InfoTable DirList(NUM);
    for(i=0;i<NUM;i++){
        strcpy(FIFO[i],("my_fifo"+to_string(i)).c_str());
        if ( mkfifo(FIFO[i],PERMS) == -1 ) {
            if (errno != EEXIST) {
                perror("receiver : mkfifo ");
                exit(1);
            }
        }
        if ((fd[i]= open(FIFO[i], O_RDWR)) < 0) {
            perror(" fifo open problem ");
            exit(4);
        }
    }
    cout << "#Parent#" << " process ID: " << getpid() << ", parent ID: " << getppid() << endl;
    for (i=0;i<NUM;i++){
        childpid = fork();
        if (childpid == -1){
            perror("fork");
            exit(2);
        }
        if (childpid == 0){
            cout << "Child #" << i+1 << ", process ID: " << getpid() << ", parent ID: " << getppid() << endl;
            retval = execlp("../Monitor", FIFO[i] , NULL);
            if(retval == -1) {
                perror("execlp");
                exit(3);
            }
            exit(0);
        }
        sleep(1);
    }
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
//        cout << "DirList Info: " << DirList.getCapacity(0) << ", " << DirList.getCapacity(1) << ", " << DirList.getCapacity(2) << endl;
//        cout << "DirList Entry Test: 1-1->" << DirList.getEntry(0,2) << ", 2-1->" << DirList.getEntry(1,2) << ", 3-1->" << DirList.getEntry(2,2) << endl;
    }
    for (i=0;i<NUM;i++) {
        wait(NULL);
    }
    for (i=0;i<NUM;i++){
        close(fd[i]);
    }
    exit(0);
}