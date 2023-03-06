#pragma once

//define color names in order
typedef enum {
    BLACK = 0,
    WHITE,
    DARK_BLUE,
    RED,
    DARK_GREEN,
    PURPLE,
    YELLOW,
    GRAY,
    LIGHT_BLUE,
    ORANGE,
    LIGHT_GREEN,
    PINK,
    BEIGE,
    MAX_COLOR_STATES
} megaminxColor;

struct colorpack {
    int i;
    double r,g,b;
    const char* name;
};

//list of the 12 colors in R,G,B from 0.0-1.0(0-255)
constexpr colorpack g_colorRGBs[MAX_COLOR_STATES] = {
    { 0, 0.0, 0.0, 0.0, "BLACK" },
    { 1, 1.0, 1.0, 1.0, "WHITE" },
    { 2, 0.0, 0.0, 1.0, "DARK_BLUE" },
    { 3, 1.0, 0.0, 0.0, "RED" },
    { 4, 0.0, 0.4, 0.0, "DARK_GREEN" },
    { 5, 0.5, 0.0, 1.0, "PURPLE" },
    { 6, 1.0, 1.0, 0.0, "YELLOW" },
    { 7, 0.5, 0.5, 0.5, "GRAY" },
    { 8, 0.2, 0.5, 1.0, "LIGHT_BLUE" },
    { 9, 1.0, 0.4, 0.1, "ORANGE" },
    { 10, 0.4, 1.0, 0.4, "LIGHT_GREEN" },
    { 11, 0.9, 0.4, 1.0, "PINK" },
    { 12, 1.0, 0.9, 0.65, "BEIGE" }
};

//Struct that can hold up to 3 Colors for Center/Edge/Corner
struct colorpiece {
    megaminxColor a = BLACK;
    megaminxColor b = BLACK;
    megaminxColor c = BLACK;
};

static const int numFaces = 12;
static const int numCorners = 20;
static const int numEdges = 30;

//Defines the 30 edge pieces.
constexpr colorpiece g_edgePiecesColors[numEdges] = {
    // 0 - 4
    { WHITE, DARK_BLUE },
    { WHITE, RED },
    { WHITE, DARK_GREEN },
    { WHITE, PURPLE },
    { WHITE, YELLOW },
    // 5 - 9
    { DARK_BLUE, RED },
    { RED, DARK_GREEN },
    { DARK_GREEN, PURPLE },
    { PURPLE, YELLOW },
    { YELLOW, DARK_BLUE },
    // 10 - 14
    { DARK_BLUE, LIGHT_GREEN },
    { RED, PINK },
    { DARK_GREEN, BEIGE },
    { PURPLE, LIGHT_BLUE },
    { YELLOW, ORANGE },
    // 15 - 19
    { DARK_BLUE, PINK },
    { RED, BEIGE },
    { DARK_GREEN, LIGHT_BLUE },
    { PURPLE, ORANGE },
    { YELLOW, LIGHT_GREEN },
    // 20 - 24
    { PINK, BEIGE },
    { BEIGE, LIGHT_BLUE },
    { LIGHT_BLUE, ORANGE },
    { ORANGE, LIGHT_GREEN },
    { LIGHT_GREEN, PINK },
    // 25 - 29
    { GRAY, LIGHT_BLUE },
    { GRAY, ORANGE },
    { GRAY, LIGHT_GREEN },
    { GRAY, PINK },
    { GRAY, BEIGE }
};

//Defines the 20 corner Pieces.
constexpr colorpiece g_cornerPiecesColors[numCorners] = {
    // 0 - 4
    { WHITE, RED, DARK_BLUE },
    { WHITE, DARK_GREEN, RED },
    { WHITE, PURPLE, DARK_GREEN },
    { WHITE, YELLOW, PURPLE },
    { WHITE, DARK_BLUE, YELLOW },
    // 5 - 9
    { DARK_BLUE, RED, PINK },   //D_blue and red faces share corner #3
    { RED, DARK_GREEN, BEIGE },
    { DARK_GREEN, PURPLE, LIGHT_BLUE },
    { PURPLE, YELLOW, ORANGE },
    { YELLOW, DARK_BLUE, LIGHT_GREEN },
    // 10 - 14
    { LIGHT_BLUE, BEIGE, DARK_GREEN },  //L_blue and beige faces share corner #2
    { BEIGE, PINK, RED },
    { PINK, LIGHT_GREEN, DARK_BLUE },
    { LIGHT_GREEN, ORANGE, YELLOW },
    { ORANGE, LIGHT_BLUE, PURPLE },
    // 15 - 19
    { GRAY, BEIGE, LIGHT_BLUE },
    { GRAY, LIGHT_BLUE, ORANGE },
    { GRAY, ORANGE, LIGHT_GREEN },
    { GRAY, LIGHT_GREEN, PINK },
    { GRAY, PINK, BEIGE }
};

//struct that holds the relative position & direction for colors in g_faceNeighbors below
//Initialized to BLACK (0)
struct colordirs {
    //order: Start from front face, then 9-oclock and going CW right around and down
    megaminxColor front = BLACK;
    megaminxColor left  = BLACK;
    megaminxColor up    = BLACK;
    megaminxColor right = BLACK;
    megaminxColor downr = BLACK;
    megaminxColor downl = BLACK;
    megaminxColor bottom = BLACK;
};

//Defines which faces are touching each other. For Human Algo Rotate.
constexpr colordirs g_faceNeighbors[MAX_COLOR_STATES] = {
    //Initialize the 0 slot to BLACK invalid
    { BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK },
    //bottom/white 1-6
    { WHITE, DARK_BLUE, RED, DARK_GREEN, PURPLE, YELLOW, ORANGE },
    { DARK_BLUE, RED, WHITE, YELLOW, LIGHT_GREEN, PINK, GRAY },
    { RED, DARK_GREEN, WHITE, DARK_BLUE, PINK, BEIGE, GRAY },
    { DARK_GREEN, PURPLE, WHITE, RED, BEIGE, LIGHT_BLUE, GRAY },
    { PURPLE, YELLOW, WHITE, DARK_GREEN, LIGHT_BLUE, ORANGE, GRAY },
    { YELLOW, DARK_BLUE, WHITE, PURPLE, ORANGE, LIGHT_GREEN, GRAY },
    //Top/gray 7-12
    { GRAY, PINK, LIGHT_GREEN, ORANGE, LIGHT_BLUE, BEIGE, DARK_GREEN },
    { LIGHT_BLUE, BEIGE, GRAY, ORANGE, PURPLE, DARK_GREEN, WHITE },
    { ORANGE, LIGHT_BLUE, GRAY, LIGHT_GREEN, YELLOW, PURPLE, WHITE },
    { LIGHT_GREEN, ORANGE, GRAY, PINK, DARK_BLUE, YELLOW, WHITE },
    { PINK, LIGHT_GREEN, GRAY, BEIGE, RED, DARK_BLUE, WHITE },
    { BEIGE, PINK, GRAY, LIGHT_BLUE, DARK_GREEN, RED, WHITE }
};

//Determine which direction those faces need to rotate to land the Edge on the white
//Decides which direction, up or down, for the pieces to passively float to their original home
constexpr int DirToWhiteFace[numFaces][5] =
{
    { 0, 0, 0, 0, 0 },
    { -1, -1, 1, 1, -1 },  //e2&3 swapped @ D.Blue
    { -1, 1, -1, 1, -1 },
    { -1, 1, -1, 1, -1 },
    { -1, 1, -1, 1, -1 },
    { -1, 1, -1, 1, -1 },
    { 0, 0, 0, 0, 0 },
    { -1, 1, 1, -1, -1 },
    { -1, 1, 1, -1, -1 },
    { -1, 1, 1, -1, -1 },
    { -1, 1, -1, 1, -1 }, //e3&4 swapped @ Pink
    { -1, 1, 1, -1, -1 }
};

//Edges that have their solved-Face-Color in the color[1] index. (marked by 1's),
//                                instead of the color[0] index. (marked by 0's)
constexpr int BlackEdgesNumber2[numFaces][5] =
{
    { 0, 0, 0, 0, 0 },
    { 1, 0, 1, 0, 0 },
    { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 0, 1 },
    { 1, 1, 1, 0, 1 },
    { 1, 1, 1, 0, 1 },
    { 1, 1, 0, 1, 1 },
    { 1, 1, 1, 0, 1 }
};