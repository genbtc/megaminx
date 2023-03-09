#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui/linenoise.h"
#include "engine/megaminx.hpp"

// include prototypes
void menuHandler(int num);

void readlineCompletion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc,"hello");
        linenoiseAddCompletion(lc,"hello there");
    }
}
void readlineShell() {
    char *line;    
    linenoiseSetCompletionCallback(readlineCompletion);
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */
    printf("Megaminx Command Line Internals - \n");
    printf("Readline Shell, starting...\n");
    while((line = linenoise("megaminx> ")) != NULL) {
        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/') {
            printf("echo: '%s'\n", line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!strncmp(line,"/historylen",11)) {
            /* The "/historylen" command will change the history len. */
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        } else if (!strncmp(line,"/hello",6)) {
            printf("Hello world!\n");
        } else if (!strncmp(line,"/menu",5)) {
            int num = atoi(line+5);
            printf("Executing Menu #%d\n", num);
            menuHandler(num);
        } else if (!strncmp(line,"/algo",5)) {
            int num = atoi(line+5);
            megaminx->rotateAlgo(num);
        } else if (!strncmp(line,"/exit",5)) {
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if (!strncmp(line,"/quit",5)) {
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