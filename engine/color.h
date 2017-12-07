#pragma once

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
    COLOR_STATES
} megaminxColor;

struct colorpack {
    int i;
    double r;
    double g;
    double b;
    const wchar_t* name;
};

//list of the 12 colors in R,G,B from 0.0-1.0(0-255)
constexpr colorpack g_colorRGBs[COLOR_STATES] = {
    { 0, 0.0, 0.0, 0.0, L"BLACK" },
    { 1, 1.0, 1.0, 1.0, L"WHITE" },
    { 2, 0.0, 0.0, 1.0, L"DARK_BLUE" },
    { 3, 1.0, 0.0, 0.0, L"RED" },
    { 4, 0.0, 0.5, 0.0, L"DARK_GREEN" },
    { 5, 0.5, 0.0, 1.0, L"PURPLE" },
    { 6, 1.0, 1.0, 0.0, L"YELLOW" },
    { 7, 0.5, 0.5, 0.5, L"GRAY" },
    { 8, 0.2, 0.5, 1.0, L"LIGHT_BLUE" },
    { 9, 1.0, 0.4, 0.1, L"ORANGE" },
    { 10, 0.3, 1.0, 0.3, L"LIGHT_GREEN" },
    { 11, 0.9, 0.4, 1.0, L"PINK" },
    { 12, 1.0, 0.94, 0.72, L"BEIGE" }
};

//Struct that can hold 2, 3 or 6 named Colors (member name a-f is irrelevant)
struct colorpiece {
    megaminxColor a = BLACK;
    megaminxColor b = BLACK;
    megaminxColor c = BLACK;
    megaminxColor d = BLACK;
    megaminxColor e = BLACK;
    megaminxColor f = BLACK;
};

//Defines the 30 edge pieces.
constexpr colorpiece g_edgePiecesColors[30] = {
    { WHITE, DARK_BLUE },
    { WHITE, RED },
    { WHITE, DARK_GREEN },
    { WHITE, PURPLE },
    { WHITE, YELLOW },
    { DARK_BLUE, RED },
    { RED, DARK_GREEN },
    { DARK_GREEN, PURPLE },
    { PURPLE, YELLOW },
    { YELLOW, DARK_BLUE },
    { DARK_BLUE, LIGHT_GREEN },
    { RED, PINK },
    { DARK_GREEN, BEIGE },
    { PURPLE, LIGHT_BLUE },
    { YELLOW, ORANGE },
    { DARK_BLUE, PINK },
    { RED, BEIGE },
    { DARK_GREEN, LIGHT_BLUE },
    { PURPLE, ORANGE },
    { YELLOW, LIGHT_GREEN },
    { PINK, BEIGE },
    { BEIGE, LIGHT_BLUE },
    { LIGHT_BLUE, ORANGE },
    { ORANGE, LIGHT_GREEN },
    { LIGHT_GREEN, PINK },
    { GRAY, LIGHT_BLUE },
    { GRAY, ORANGE },
    { GRAY, LIGHT_GREEN },
    { GRAY, PINK },
    { GRAY, BEIGE }
};

//Defines the 20 corner Pieces.
constexpr colorpiece g_cornerPiecesColors[20] = {
    { WHITE, RED, DARK_BLUE },
    { WHITE, DARK_GREEN, RED },
    { WHITE, PURPLE, DARK_GREEN },
    { WHITE, YELLOW, PURPLE },
    { WHITE, DARK_BLUE, YELLOW },
    { DARK_BLUE, RED, PINK },
    { RED, DARK_GREEN, BEIGE },
    { DARK_GREEN, PURPLE, LIGHT_BLUE },
    { PURPLE, YELLOW, ORANGE },
    { YELLOW, DARK_BLUE, LIGHT_GREEN },
    { GRAY, BEIGE, LIGHT_BLUE },
    { GRAY, LIGHT_BLUE, ORANGE },
    { GRAY, ORANGE, LIGHT_GREEN },
    { GRAY, LIGHT_GREEN, PINK },
    { GRAY, PINK, BEIGE },
    { LIGHT_BLUE, BEIGE, DARK_GREEN },
    { BEIGE, PINK, RED },
    { PINK, LIGHT_GREEN, DARK_BLUE },
    { LIGHT_GREEN, ORANGE, YELLOW },
    { ORANGE, LIGHT_BLUE, PURPLE }
};

//struct that holds the relational directional colors for g_faceNeighbors
struct colordirs {
    //Starting from face,then 9-oclock and going CW right around.
    megaminxColor front = BLACK;
    megaminxColor left = BLACK;
    megaminxColor up = BLACK;
    megaminxColor right = BLACK;
    megaminxColor downr = BLACK;
    megaminxColor downl = BLACK;
};

//Defines which faces are touching each other. For Human Algo Rotate.
constexpr colordirs g_faceNeighbors[COLOR_STATES] = {
    //fill up the 0 slot
    { BLACK, BLACK, BLACK, BLACK, BLACK, BLACK },
    //bottom/white 1-6
    { WHITE,DARK_BLUE,RED,DARK_GREEN,PURPLE, YELLOW },
    { DARK_BLUE,RED,WHITE,YELLOW,LIGHT_GREEN,PINK },
    { RED, DARK_GREEN, WHITE, DARK_BLUE, PINK, BEIGE },
    { DARK_GREEN, PURPLE, WHITE, RED, BEIGE, LIGHT_BLUE },
    { PURPLE, YELLOW, WHITE, DARK_GREEN, LIGHT_BLUE, ORANGE },
    { YELLOW, DARK_BLUE,WHITE, PURPLE, ORANGE,LIGHT_GREEN },
    //Top/gray 7-12
    { GRAY, LIGHT_BLUE,ORANGE,LIGHT_GREEN,PINK,BEIGE },
    { LIGHT_BLUE,BEIGE,GRAY,ORANGE,PURPLE,DARK_GREEN },
    { ORANGE, LIGHT_BLUE,GRAY,LIGHT_GREEN,YELLOW, PURPLE },
    { LIGHT_GREEN,ORANGE,GRAY,PINK,DARK_BLUE,YELLOW },
    { PINK, LIGHT_GREEN,GRAY,BEIGE,RED,DARK_BLUE },
    { BEIGE, PINK,GRAY,LIGHT_BLUE,DARK_GREEN,RED }
};