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
    if(wdebug!=NULL){\
        wrefresh(wdebug);\
        wrefresh(wdebugbord);\
    }\
})

#define MAXDEBUGLEN 256

WINDOW *wmain;
WINDOW *wdebug;
WINDOW *wdebugbord;

WINDOW *pbar;

static bool colors, initialized = false;
static int mainh, mainw, debw, debh, deblines;

static char* debugcontents;

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
        debw = (COLS/3)-2;
        debh = mainh-2;
        deblines = 0;
        wdebug= newwin(deblines, debw, 1, mainw+1);
        /* FIXME: buffer should be more flexible */
        debugcontents = (char *) calloc(deblines*debw, sizeof(char));
        wdebugbord = newwin(mainh, debw+2, 0, mainw);
        box(wdebugbord, 0, 0);
    } else {
        wdebug = NULL;
        wdebugbord = NULL;
    }

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

void printDebugLine(char *msg){
    int i, t, j, lines = 1, bsize;
    char *tmp;

    wclear(wdebug);

    for(i = 0, t = 0; i < strlen(msg); i++, t++){
        if(t >= debw){
            lines++;
            t = 0;
        }
    }

    tmp = (char *) calloc(strlen(msg)+lines, sizeof(char));
    for(i = 0, t = 0, j = 0; i <= strlen(msg); i++, t++, j++){
        if(t >= debw){
            tmp[j] = '\n';
            j++;
            t = 0;
        }
        tmp[j] = msg[i];
    }

    while((lines+deblines) >= debh){
        t = 0;
        bsize = strlen(debugcontents);
        while(debugcontents[t] != '\0' && debugcontents[t] != '\n')
            t++;
        t++;
        for(i = 0; i < (bsize-t); i++){
            debugcontents[i] = debugcontents[i+t];
        }
        deblines--;
    }

    strcat(debugcontents, tmp);
    deblines += lines;

    free(tmp);

    mvwprintw(wdebug, 0, 0, "%s", debugcontents);
    wrefresh(wdebug);
}

void debugPrint(const char *f, ...){
    char *str, *tmp;
    int i, slen, j;
    va_list params;
    va_start(params, f);
    if(initialized){
        vasprintf(&str, f, params);
    } else
        vprintf(f, params);
    va_end(params);

    if(initialized){
        slen = strlen(str);
        if(slen){
            tmp = (char *) calloc (slen, sizeof(char));
            for(i = 0, j = 0; i < slen; i++, j++){
                tmp[j] = str[i];
                if(tmp[j] == '\n'){
                    tmp[j+1] = '\0';
                    printDebugLine(tmp);
                    j=0;
                }
            }
            if(j>1){
                tmp[j] = '\0';
                printDebugLine(tmp);
            }
            free(tmp);
        }
        free(str);
    }
}

void closeTui(){
    if(initialized){
        if(wmain != NULL)
            delwin(wmain);
        if(wdebug != NULL){
            CLEAR_BORDER(wdebugbord);
            delwin(wdebugbord);
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
