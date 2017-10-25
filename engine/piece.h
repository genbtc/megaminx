#ifndef _PIECE_H_
#define _PIECE_H_

typedef enum {
    WHITE = 0,
    YELLOW,
    BLUE,
    RED,
    GREEN,
    PURPLE,
    LIGHT_GREEN,
    PINK,
    BONE,
    LIGHT_BLUE,
    ORANGE,
    GRAY,
    COLOR_STATES
} g_enum_COLOR_STATES;

static const wchar_t* g_colornames[COLOR_STATES] = {
    L"WHITE",
    L"YELLOW",          /* other colors have more precedence */
    L"BLUE",                /* has lowest precedence */
    L"RED",
    L"GREEN",
    L"PURPLE",
    L"LIGHT_GREEN",
    L"PINK",
    L"BONE",
    L"LIGHT_BLUE",
    L"ORANGE",
    L"GRAY"
};

struct color
{
    double r;
    double g;
    double b;
};
static color g_colorRGBs[COLOR_STATES] =
{
    {1.0, 1.0, 1.0},
    {0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0},
    {0.0, 0.5, 0.0},
    {0.5, 0.0, 1.0},
    {1.0, 1.0, 0.0},
    {0.5, 0.5, 0.5},
    {0.2, 0.5, 1.0},
    {1.0, 0.4, 0.1},
    {0.3, 1.0, 0.3},
    {0.9, 0.4, 1.0},
    {1.0, 1.0, 0.3}
};

class Piece
{

public:
    Piece()
    {
    }

    ~Piece() = default;
public:
    double _color[3][3];
    double _vertex[7][3];

    void setColor(int i, color c)
    {
        _color[i][0] = c.r;
        _color[i][1] = c.g;
        _color[i][2] = c.b;
    }

    void initColorIndex(int idx,int k)
    {
        const auto color = g_colorRGBs[k-1];
        setColor(idx, color);
    }
    
    void initColor(int a)
    {
        initColorIndex(0,a);
    }

    void initColor(int a, int b)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
    }
    void initColor(int a, int b, int c)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
        initColorIndex(2, c);
    }
};

#endif