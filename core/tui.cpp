/*****************************************************************************/
/* Improveesation general structs header file.                               */
/* This library is a part of the IMprovEEsation suite.                       */
/*                                                                           */
/* Copyright (C) 2014                                                        */
/* Federico Montori, Marco Melletti, Davide Berardi, Matteo Martelli         */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or any later version.                                     */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,*/
/* USA.                                                                      */
/*****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ncurses.h>

#define CLEAR_BORDER(w) (wborder(w, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '))
#define REFRESH_ALL() ({\
    wrefresh(wmain);\
    if(wdebug!=NULL)wrefresh(wdebug);\
})

#define MAXDEBUGLEN 1024

WINDOW *wmain;
WINDOW *wdebug;

WINDOW *pbar;

static bool colors, initialized = false;
static int mainh, mainw, debw;

static char* debugcontents;
static int debugi, debugs;


void initTui(bool debug){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    colors = has_colors();

    if(colors){
        start_color();
        /* TODO: define color pairs (fg,bg) */
    }

    mainh = LINES;
    mainw = COLS - (debug ? COLS/3 : 0);

    wmain = newwin(mainh, mainw, 0, 0);
    if(debug){
        debw = COLS/3;
        wdebug= newwin(mainh, debw, 0, mainw);
        /* FIXME: buffer should be more flexible */
        debugs = mainh * debw;
        debugcontents = (char *) calloc(debugs, sizeof(char));
        debugi = 0;
        box(wdebug, 0, 0);
    } else
        wdebug = NULL;

    REFRESH_ALL();

    initialized = true;
}

void makeDirectorMenu(uint32_t *musicians_count, int *measures_count){
    if(initialized){
        wclear(wmain);
    }
}

void makeProgressBar(){
    if(initialized){
        wclear(wmain);
    }
}

void advanceProgressBar(int percentage){
    if(initialized){

    }
}

void fullscreenMessage(char *message){
    if(initialized){
        wclear(wmain);
    }
}

void debugPrint(const char *f, ...){
    char *str;
    va_list params;
    va_start(params, f);
    if(initialized){
        str = (char *) calloc (MAXDEBUGLEN, sizeof(char));
        vsprintf(str, f, params);
    } else
        vprintf(f, params);
    va_end(params);

    if(initialized){
        /*delete some chunks of debug buffer*/
        while((strlen(str) + debugi) >= debugs){
            int tmpi = 0;
            while(debugcontents[tmpi] != '\n')
                tmpi++;
            debugi -= tmpi;
            for(int i = 0; i < debugi; i++)
                debugcontents[i] = debugcontents[i+tmpi];
        }

        strcpy(debugcontents+debugi, str);
        debugi += strlen(str);
        free(str);

        wclear(wdebug);
        wprintw(wdebug, debugcontents);
        wrefresh(wdebug);
    }
}

void closeTui(){
    if(initialized){
        delwin(wmain);
        if(wdebug != NULL){
            CLEAR_BORDER(wdebug);
            delwin(wdebug);
        }

        endwin();
    }
}

int waitForInput(){
    if(initialized){
        return getch();
    }
}

void waitForDialogOk(){
    if(initialized){

    }
}
