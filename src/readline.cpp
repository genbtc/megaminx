#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui/linenoise.h"
#include "engine/megaminx.hpp"

// include prototype from main-menu.cpp
void menuHandler(int num);

void readlineCompletion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc,"hello");
        linenoiseAddCompletion(lc,"hello there");
        linenoiseAddCompletion(lc,"hello world");
    }
}
void readlineShell() {
    char *line;
    linenoiseSetCompletionCallback(readlineCompletion);
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */
    printf("Megaminx Command Line Internals - \n");
    printf("Readline Shell, starting...\n");
    /* Interactive Prompt: megaminx> */
    while((line = linenoise("megaminx> ")) != NULL) {
        linenoiseHistoryAdd(line);  /* Add input line to the history file. */
        linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        if (line[0] == '\0') {
            printf("No Input, returning to GUI.\n");
            free(line);
            return;
        } else if ((!strncmp(line,"exit",4))
                || (!strncmp(line,"quit",4))) {
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if (line[0] != '/') {
            printf("not a command: '%s'\n", line);
        } else if (!strncmp(line,"/hello",6)) {
            printf("Hello World!\n");
        } else if (!strncmp(line,"/historylen",11)) {
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len); /* change the history length. */
            printf("Changed History Length to #%d lines\n", len);
        } else if (!strncmp(line,"/menu",5)) {
            int num = atoi(line+5);
            printf("Executing Menu #%d\n", num);
            menuHandler(num);
        } else if (!strncmp(line,"/algotest",9)) {
            printf("Testing All Algorithms 1-50! ...\n");
            megaminx->testingAlgostrings(shadowDom);
            free(line);
            return;
        } else if (!strncmp(line,"/algodiff",9)) {
            int num = atoi(line+9);
            for (int i=1;i<50;i++) {
                auto algo = megaminx->ParseAlgorithmID(i, LIGHT_BLUE);
                printf("Number of Moves: #%ld\n", algo.size() );
                printf("Executing Algo #%d\n", i);
                megaminx->rotateAlgo(i);
            }
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if (!strncmp(line,"/algo",5)) {
            int num = atoi(line+5);
            printf("Executing Algo #%d\n", num);
            megaminx->rotateAlgo(num);
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if ((!strncmp(line,"/exit",5))
                || (!strncmp(line,"/quit",5))) {
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if (line[0] == '/') {
            printf("Unrecognized command: %s\n", line);
        }
        free(line);
    }
    return;
}
