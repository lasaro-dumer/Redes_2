#include <iostream>
#include <curses.h>
#include "screen.hpp"

void initCurses(){
	(void) initscr();      /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void) nonl();         /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();       /* take input chars one at a time, no wait for \n */
	(void) echo();         /* echo input - in color */

	if (has_colors())
	{
		start_color();
		/*
		 * Simple color assignment, often all we need.  Color pair 0 cannot
		 * be redefined.  This example uses the same value for the color
		 * pair as for the foreground color, though of course that is not
		 * necessary:
		 */
		init_pair(1, COLOR_RED,     COLOR_BLACK);
		init_pair(2, COLOR_GREEN,   COLOR_BLACK);
		init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
		init_pair(4, COLOR_BLUE,    COLOR_BLACK);
		init_pair(5, COLOR_CYAN,    COLOR_BLACK);
		init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(7, COLOR_WHITE,   COLOR_BLACK);
	}
}

void showOutput(string text, bool clearScr){
	#ifndef PCUR
	cout << text.c_str() << endl;
	#else
	move(0,0);
	if(clearScr){
		clear();
		refresh();
	}
	move(0,0);
	attrset(COLOR_PAIR(7));
	mvprintw(0,0,text.c_str());
	// mvprintw(9,1,to_string(c).c_str());
	int x,y;
	getyx(curscr,y,x);
	move(y+1,0);
	refresh();
	#endif
}

string getstring()
{
    std::string input;

    // let the terminal do the line editing
    nocbreak();
    echo();

    // this reads from buffer after <ENTER>, not "raw"
    // so any backspacing etc. has already been taken care of
    int ch = getch();

    while ( ch != '\n' )
    {
        input.push_back( ch );
        ch = getch();
    }

    // restore your cbreak / echo settings here

    return input;
}
