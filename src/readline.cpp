#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui/linenoise.h"
#include "engine/megaminx.hpp"
#include "engine/load.hpp"

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
    std::cout << "Megaminx Command Line Internals - \n";
    std::cout << "Readline Shell, starting...\n";
    /* Interactive Prompt: megaminx> */
    while((line = linenoise("megaminx> ")) != NULL) {
        linenoiseHistoryAdd(line);  /* Add input line to the history file. */
        linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        if (line[0] == '\0') {
            std::cout << "No Input, returning to GUI.\n";
            free(line);
            return;
        } else if ((!strncmp(line,"exit",4))
                || (!strncmp(line,"quit",4))) {
            std::cout << "Exiting shell, returning to GUI\n";
            free(line);
            return;
        } else if (line[0] != '/') {
            printf("Error, not a command: %s\n", line);
        } else if (!strncmp(line,"/hello",6)) {
            std::cout << "Hello World!\n";
        } else if (!strncmp(line,"/historylen",11)) {
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
            printf("Changed History Length to #%d lines\n", len);
        } else if (!strncmp(line,"/menu",5)) {
            int num = atoi(line+5);
            printf("Executing Menu #%d\n", num);
            menuHandler(num);
        } else if (!strncmp(line,"/algotest",9)) {
            printf("Testing All Algorithms #1 - #%d! ...\n", ALL_ALGORITHMS);
            megaminx->testingAlgostrings(shadowDom);
            free(line);
            return;
        } else if (!strncmp(line,"/algodiff",9)) {
            for (int i=1; i<ALL_ALGORITHMS; i++) {
                auto algo = megaminx->ParseAlgorithmID(i, LIGHT_BLUE);
                printf("Algo #%d - Number of Moves: #%ld\n", i, algo.size() );
                //megaminx->rotateAlgo(i);
            }
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if (!strncmp(line,"/algo",5)) {
            int num = atoi(line+5);
            printf("Executing Algo #%d\n", num);
            //megaminx->rotateAlgo(num);
            megaminx->rotateBulkAlgoString(g_edgeOrientation[num].algo, g_faceNeighbors[LIGHT_BLUE], num);
            printf("Exiting shell, returning to GUI\n");
            free(line);
            return;
        } else if (!strncmp(line,"/testdir",8)) {
            printf("Running Test: %s\n", line);
            std::string testDir(line+9);
            RestoreCubeFromTEST(testDir);
        } else if (!strncmp(line,"/testconvert",12)) {
            printf("Converting Test: %s\n", line);
            std::string testDir(line+13);
            RestoreOldCubeFromFile(testDir);
        } else if (!strncmp(line,"/edgevertex",11)) {
            printf("Edge Vertexes: \n");
            const int maxpcs = megaminx->getMaxNumberOfPieces<Edge>();
            for (int r = 0; r < maxpcs; ++r) {
                double vertexArray[7][3] = { 0 };
                memcpy(vertexArray, megaminx->getPieceArray<Edge>(0)[r].getVertexData(), sizeof(vertexArray));
                for (int i = 0; i < 7; ++i)
                    printf("[ %f %f %f ]\n", vertexArray[i][0], vertexArray[i][1], vertexArray[i][2]);
            }
        } else if (!strncmp(line,"/cornervertex",13)) {
            printf("Corner Vertexes: \n");
            const int maxpcs = megaminx->getMaxNumberOfPieces<Corner>();
            for (int r = 0; r < maxpcs; ++r) {
                double vertexArray[7][3] = { 0 };
                memcpy(vertexArray, megaminx->getPieceArray<Corner>(0)[r].getVertexData(), sizeof(vertexArray));
                for (int i = 0; i < 7; ++i)
                    printf("[ %f %f %f ]\n", vertexArray[i][0], vertexArray[i][1], vertexArray[i][2]);
            }
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
