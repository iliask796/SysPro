#include "InputHandler.h"
#include <iostream>

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
    return "-1";
}

bool CommandInput::isDigit(int pos) {
    for(char i : wordArray[pos]){
        if (isdigit(i) == false){
            return false;
        }
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
    while (!wordArray[i].empty()){
        i++;
    }
    return i;
}

void CommandInput::display(){
    for (int i=0;i<size;i++){
        if (wordArray[i].empty()){
            return;
        }
        cout << wordArray[i] << endl;
    }
}

CommandInput::~CommandInput() {
    delete[] wordArray;
}