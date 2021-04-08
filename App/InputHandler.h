#include <string>
using namespace std;

class CommandInput{
private:
    int size;
    string* wordArray;
public:
    CommandInput(int);
    void insertCommand(const string&);
    string getWord(int);
    bool isDigit(int);
    void clear();
    int getCount();
    void display();
    ~CommandInput();
};