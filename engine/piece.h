#ifndef _PIECE_H_
#define _PIECE_H_
#include <math.h>
#include "vector3d.h"
#include "utils.h"

//common constants

static const long double FI = (1 + sqrt(5)) / 2;
static const long double PI = acos(-1);
static const long double SIDE_ANGLE = 2 * atan(FI);
static const long double INS_SPHERE_RAD = 100 * sqrt(10 + 22 / sqrt(5)) / 4;
static const long double INS_CIRCLE_RAD = 100 / sqrt((5 - sqrt(5)) / 2);
#define pim(x) x*PI/5

typedef enum {
    WHITE = 0,
    BLUE,
    RED,
    GREEN,
    PURPLE,
    YELLOW,
    GRAY,
    LIGHT_BLUE,
    ORANGE,
    LIGHT_GREEN,
    PINK,
    BONE,
    COLOR_STATES
} g_enum_COLOR_STATES;

struct color
{
    double r;
    double g;
    double b;
    const wchar_t* name;
};

//list of the 12 colors in R,G,B from 0.0-1.0(0-255)
static color g_colorRGBs[COLOR_STATES] =
{                    
    { 1.0, 1.0, 1.0, L"WHITE" },
    { 0.0, 0.0, 1.0, L"BLUE" },
    { 1.0, 0.0, 0.0, L"RED" },
    { 0.0, 0.5, 0.0, L"GREEN" },
    { 0.5, 0.0, 1.0, L"PURPLE" },
    { 1.0, 1.0, 0.0, L"YELLOW" },
    { 0.5, 0.5, 0.5, L"GRAY" },
    { 0.2, 0.5, 1.0, L"LIGHT_BLUE"},
    { 1.0, 0.4, 0.1, L"ORANGE" },
    { 0.3, 1.0, 0.3, L"LIGHT_GREEN" },
    { 0.9, 0.4, 1.0, L"PINK" },
    { 1.0, 1.0, 0.8, L"BONE" }
};
struct piecepack
{
    char axis1, axis2;
    int multi;
};
class Piece
{
public:
    //no constructor 
    Piece() { }
    //no destructor
    virtual ~Piece() = default;

    //data-members
    double _color[3][3];
    double _vertex[7][3];
    Vector3d v3d[7];
    const wchar_t* _colorName;
    //getter
    double* getcolor()
    {
        return &_color[0][0];
    }
    //setter
    void setColor(int i, ::color c)
    {
        _color[i][0] = c.r;
        _color[i][1] = c.g;
        _color[i][2] = c.b;
        _colorName = c.name;
    }
    //common
    void initColorIndex(int idx,int k)
    {
        const auto color = g_colorRGBs[k-1];
        setColor(idx, color);
    }
    //store center color
    void initColor(int a)
    {
        initColorIndex(0,a);
    }
    //store edge colors
    void initColor(int a, int b)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
    }
    //store corner colors
    void initColor(int a, int b, int c)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
        initColorIndex(2, c);
    }
    static void axis1multi(double* target, piecepack &pack)
    {
        rotateVertex(target, pack.axis1, pim(pack.multi));
    }
    static void CenterSide1(double* target, piecepack &pack)
    {
        rotateVertex(target, pack.axis1, pim(1));
        rotateVertex(target, pack.axis2, PI - SIDE_ANGLE);
        axis1multi(target, pack);
    }
    static void CenterCenter(double* target, piecepack &pack)
    {
        rotateVertex(target, pack.axis1, PI);
    }
    static void CenterSide2(double* target, piecepack &pack)
    {
        CenterCenter(target, pack);
        rotateVertex(target, pack.axis2, PI - SIDE_ANGLE);
        rotateVertex(target, 'z', pim(pack.multi));
        //This is always z, because axis1/2 are usually y/x and 
        //is re-used by face, where it is Z.
    }
    static void CornerGrp3(double* target, piecepack &pack)
    {
        rotateVertex(target, pack.axis1, PI);
        rotateVertex(target, pack.axis2, pim(pack.multi));
    }
    static void CornerGrp4(double* target, piecepack &pack)
    {
        CenterSide1(target, pack);
        rotateVertex(target, pack.axis2, PI);
    }
    static void EdgeGrp2(double* target, piecepack &pack)
    {
        rotateVertex(target, 'z', pim(3));
        rotateVertex(target, 'x', PI - SIDE_ANGLE);
        axis1multi(target, pack);
    }
    static void EdgeGrp3(double* target, piecepack &pack)
    {
        rotateVertex(target, 'z', pim(6));
        EdgeGrp2(target, pack);
    }
    static void EdgeGrp4(double* target, piecepack &pack)
    {
        rotateVertex(target, 'z', pim(8));
        EdgeGrp2(target, pack);
    }
    static void EdgeGrp5(double* target, piecepack &pack)
    {
        rotateVertex(target, 'z', pim(2));
        rotateVertex(target, 'x', SIDE_ANGLE);
        axis1multi(target, pack);
    }
    static void EdgeGrp6(double* target, piecepack &pack)
    {
        rotateVertex(target, 'x', PI);
        axis1multi(target, pack);
    }
};

#endif