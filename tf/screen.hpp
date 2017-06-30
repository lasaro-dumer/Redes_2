#ifndef screenH
#define screenH
#include <string>
using namespace std;

void initCurses();
void showOutput(string text, bool clearScr = false);
string getstring();
#endif
