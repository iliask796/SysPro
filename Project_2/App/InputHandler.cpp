#include "InputHandler.h"
#include <iostream>
using namespace std;

CommandInput::CommandInput(int size1) {
    size = size1;
    wordArray = new string[size];
}

void CommandInput::insertCommand(const string& command) {
    int counter = 0;
    int start = 0;
    int end = command.find(' ');
    while (end != -1){
        wordArray[counter++] = command.substr(start,end-start);
        start = end + 1;
        end = command.find(' ',start);
        if (counter == size-1){
            break;
        }
    }
    wordArray[counter] = command.substr(start,end-start);
}

string CommandInput::getWord(int pos) {
    if (pos>=0 and pos<=size-1){
        return wordArray[pos];
    }
    return "";
}

bool CommandInput::isDigit(int pos) {
    for(char i : wordArray[pos]){
        if (isdigit(i) == false){
            return false;
        }
    }
    return true;
}

bool CommandInput::isDate(int pos) {
    int i;
    string date = wordArray[pos];
    if (date.length() < 8 or date.length() > 10){
        return false;
    }
    if (isdigit(date[0]) == false){
        return false;
    }
    for (i=1;i<4;i++){
        if (isdigit(date[date.length()-i]) == false){
            return false;
        }
    }
    if (date[date.length()-(++i)] != '-' or (date[2] != '-' and date[3]!= '-')){
        return false;
    }
    return true;
}

void CommandInput::clear() {
    for (int i=0;i<size;i++){
        wordArray[i] = "";
    }
}

int CommandInput::getCount() {
    int i=0;
    while (i<size){
        if (wordArray[i].empty()){
            break;
        }
        i++;
    }
    return i;
}

CommandInput::~CommandInput() {
    delete[] wordArray;
}

int compareDates(const string& date1,const string& date2) {
    string dateInfo1[3];
    string dateInfo2[3];
    int i,start = 0,end = date1.find('-');
    for (i=0;i<2;i++){
        dateInfo1[i]= date1.substr(start,end-start);
        start = end + 1;
        end = date1.find('-',start);
    }
    dateInfo1[i]= date1.substr(start,end-start);
    start = 0;
    end = date2.find('-');
    for (i=0;i<2;i++){
        dateInfo2[i]= date2.substr(start,end-start);
        start = end + 1;
        end = date2.find('-',start);
    }
    dateInfo2[i]= date2.substr(start,end-start);
    if (dateInfo1[2]>dateInfo2[2]){
        return -1;
    }
    else if (dateInfo1[2]<dateInfo2[2]){
        return 1;
    }
    else{
        if (dateInfo1[1]>dateInfo2[1]){
            return -1;
        }
        else if (dateInfo1[1]<dateInfo2[1]){
            return 1;
        }
        else{
            if (dateInfo1[0]>dateInfo2[0]){
                return -1;
            }
            else if (dateInfo1[0]<dateInfo2[0]){
                return 1;
            }
            else{
                return 0;
            }
        }
    }
}

int safeDateCheck(const string& date1,const string& date2) {
    string dateInfo1[3];
    string dateInfo2[3];
    int year1,year2,days1,days2;
    int i,start = 0,end = date1.find('-');
    for (i=0;i<2;i++){
        dateInfo1[i]= date1.substr(start,end-start);
        start = end + 1;
        end = date1.find('-',start);
    }
    dateInfo1[i]= date1.substr(start,end-start);
    start = 0;
    end = date2.find('-');
    for (i=0;i<2;i++){
        dateInfo2[i]= date2.substr(start,end-start);
        start = end + 1;
        end = date2.find('-',start);
    }
    dateInfo2[i]= date2.substr(start,end-start);
    days1 = stoi(dateInfo1[0])+30*stoi(dateInfo1[1]);
    year1 = stoi(dateInfo1[2]);
    days2 = stoi(dateInfo2[0])+30*stoi(dateInfo2[1]);
    year2 = stoi(dateInfo2[2]);
    if (year2>year1){
        return 1;
    }
    else{
        if (year2==year1 and days2-days1>=180){
            return 1;
        }
        else{
            return 0;
        }
    }
}