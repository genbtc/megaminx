#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include "color.h"
#include "../ui/opengl.h"

using std::acos;
using std::atan;
using std::cos;
using std::sin;
using std::sqrt;

//common geometric constants
static const long double DODESIZE = 100;
static const long double PI = acos(-1.f);           //3.1415927410125732
//Golden Ratio (Phi) (also The ratio between the side length of a regular pentagon and one of its diagonals.)
static const long double FI = (1 + sqrt(5.f)) / 2;  //1.6180340051651001
static const long double SIDE_ANGLE = 2 * atan(FI); //2.0344439448698051
//inscribed sphere radius ( ri = a / 2 * √ ( 25 + 11 * √5 ) / 10 )
static const long double INS_SPHERE_RAD = DODESIZE * sqrt(10+22 / sqrt(5.f)) / 4;   //111.35163307189941 
static const long double INS_CIRCLE_RAD = DODESIZE / sqrt((5 - sqrt(5.f)) / 2);     // 85.065082037033278
#define pim(x) x*PI/5
//megaminx vertex shortcuts
static const long double TWOFIFTHS = (double)2/5;
static const long double EDGEFIFTH = DODESIZE / sin(pim(2));            //105.14622122913930
static const long double COSPIM35 = INS_CIRCLE_RAD * cos(pim(3.5));     //-50.000004917867173
static const long double COSPIM15 = INS_CIRCLE_RAD * cos(pim(1.5));     //49.999998901510480
static const long double SINPIM35 = INS_CIRCLE_RAD * sin(pim(3.5));     //68.819093936061520
void rotateVertex(double *vertex, char axis, double angle);

struct piecepack {
    char axis1, axis2;
    int multi;
};
class Piece {
public:
    //virtual destructor
    virtual ~Piece() = default;

    //Coords for GL vertex (up to 7, not all used)
    double _vertex[7][3];
    //Keeps the default number in the piece. do not swap.
    int defaultPieceNum = NULL;

    //data-members we can swap out
    struct _data {
        double _color[3][3];
        int _colorNum[3];
        const wchar_t* _colorName[3];
        int pieceNum;
        int flipStatus;
    } data;

    //Center has 1, Edge has 2, Corner has 3
    int numSides;

    //Swaps current data with the &out_param
    void swapdata(_data &out) {
        if (&(this->data) == &out)
            return;
        const _data temp = data;
        data = out;
        out = temp;
    }
    //getter
    double* getcolor() {
        return &data._color[0][0];
    }
    //setter
    void setColor(int i, colorpack c) {
        data._color[i][0] = c.r;
        data._color[i][1] = c.g;
        data._color[i][2] = c.b;
        data._colorName[i] = c.name;
        data._colorNum[i] = c.i;
        data.flipStatus = 0;
    }
    //interface function for setter
    void initColorIndex(int idx, int k) {
        const colorpack thecolor = g_colorRGBs[k];
        setColor(idx, thecolor);
    }
    //store Center color
    void initColor(int a) {
        initColorIndex(0, a);
        numSides = 1;
    }
    //store Edge colors
    void initColor(int a, int b) {
        initColorIndex(0, a);
        initColorIndex(1, b);
        //set non-existant 3rd side of edge to
        // 0==black aka not undefined so we can re-use corner.
        initColorIndex(2, 0);
        numSides = 2;
    }
    //store Corner colors
    void initColor(int a, int b, int c) {
        initColorIndex(0, a);
        initColorIndex(1, b);
        initColorIndex(2, c);
        numSides = 3;
    }
    //Use the two arrays g_cornerPiecesColors and g_edgePiecesColors to populate.
    void initColor(colorpiece color,int corner=0) {
        initColorIndex(0, color.a);
        initColorIndex(1, color.b);
        if (corner) {
            initColorIndex(2, color.c);
            numSides = 3;
        }
        else { //edge
            initColorIndex(2, 0);   //set 3rd side to black.
            numSides = 2;
        }
    }
    //check if color-num (int) matches any colors
    // currently stored in struct data (3 sided)
    bool matchesColor(int color) const {
        return  data._colorNum[0] == color ||
                data._colorNum[1] == color ||
                data._colorNum[2] == color;
    }

    //Changes colors. Flip/rotate/switches colors for current piece.
    void flip() {
        const bool isCorner = (numSides == 3);
        double buf[3];
        for (int i = 0; i < 3; ++i) buf[i] = data._color[0][i];
        for (int i = 0; i < 3; ++i) data._color[0][i] = data._color[1][i];
        for (int i = 0; i < 3; ++i) data._color[1][i] = buf[i];
        if (isCorner) {
            for (int i = 0; i < 3; ++i) buf[i] = data._color[1][i];
            for (int i = 0; i < 3; ++i) data._color[1][i] = data._color[2][i];
            for (int i = 0; i < 3; ++i) data._color[2][i] = buf[i];
        }
        const wchar_t *namebuf;
        namebuf = data._colorName[0];
        data._colorName[0] = data._colorName[1];
        data._colorName[1] = namebuf;
        if (isCorner) {
            namebuf = data._colorName[1];
            data._colorName[1] = data._colorName[2];
            data._colorName[2] = namebuf;
        }
        int numbuff;
        numbuff = data._colorNum[0];
        data._colorNum[0] = data._colorNum[1];
        data._colorNum[1] = numbuff;
        if (isCorner) {
            numbuff = data._colorNum[1];
            data._colorNum[1] = data._colorNum[2];
            data._colorNum[2] = numbuff;
        }
        if (isCorner && data.flipStatus < 2 || !isCorner && data.flipStatus == 0)
            data.flipStatus++;
        else
            data.flipStatus = 0;
    }
    //Does two flips. Thats it.
    void flipTwice() {
        flip();
        flip();
    }
    //Creates the common starting vertexes for all pieces that are CORNERS
    double* cornerInit() {
        numSides = 3;
        for (int i = 0; i < 7; ++i) {
            _vertex[i][2] = -INS_SPHERE_RAD;
        }

        _vertex[0][0] = COSPIM35 * TWOFIFTHS; //inside corner (aka outside center)
        _vertex[0][1] = SINPIM35 * TWOFIFTHS;

        _vertex[1][0] = COSPIM35 + EDGEFIFTH * TWOFIFTHS; //corner inside edge a
        _vertex[1][1] = SINPIM35;

        _vertex[2][0] = COSPIM35;     //outside corner
        _vertex[2][1] = SINPIM35;

        _vertex[3][0] = COSPIM15 - EDGEFIFTH * TWOFIFTHS; //corner inside edge b
        _vertex[3][1] = SINPIM35;
        rotateVertex(_vertex[3], 'z', pim(2));

        _vertex[4][0] = COSPIM15 * TWOFIFTHS; //brother = 0 or 6
        _vertex[4][1] = SINPIM35 * TWOFIFTHS;
        rotateVertex(_vertex[4], 'z', pim(-3));
        rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);
        rotateVertex(_vertex[4], 'z', pim(2));

        _vertex[5][0] = COSPIM15 - EDGEFIFTH * TWOFIFTHS; //brother = 3 or 1
        _vertex[5][1] = SINPIM35;
        rotateVertex(_vertex[5], 'z', pim(-3));
        rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
        rotateVertex(_vertex[5], 'z', pim(2));

        _vertex[6][0] = COSPIM15 * TWOFIFTHS; //brother = 0 or 4
        _vertex[6][1] = SINPIM35 * TWOFIFTHS;
        rotateVertex(_vertex[6], 'z', pim(-5));
        rotateVertex(_vertex[6], 'x', PI - SIDE_ANGLE);
        return &_vertex[0][0];
    }
    //Creates the common starting vertexes for all pieces that are EDGES
    double* edgeInit() {
        numSides = 2;
        for (int i = 0; i < 6; ++i) {
            _vertex[i][2] = -INS_SPHERE_RAD;
        }

        _vertex[0][0] = COSPIM35 * TWOFIFTHS;
        _vertex[0][1] = SINPIM35 * TWOFIFTHS;

        _vertex[1][0] = COSPIM15 * TWOFIFTHS;
        _vertex[1][1] = SINPIM35 * TWOFIFTHS;

        _vertex[2][0] = COSPIM15 - EDGEFIFTH * TWOFIFTHS;
        _vertex[2][1] = SINPIM35;

        _vertex[3][0] = COSPIM35 + EDGEFIFTH * TWOFIFTHS;
        _vertex[3][1] = SINPIM35;

        _vertex[4][0] = _vertex[1][0];
        _vertex[4][1] = _vertex[1][1];
        rotateVertex(_vertex[4], 'z', PI);
        rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);

        _vertex[5][0] = _vertex[0][0];
        _vertex[5][1] = _vertex[0][1];
        rotateVertex(_vertex[5], 'z', PI);
        rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
        return &_vertex[0][0];
    }
    //Creates the common starting vertexes for all pieces that are CENTERS
    double* centerInit() {
        numSides = 1;
        for (int i = 0; i < 5; ++i) {
            _vertex[i][0] = INS_CIRCLE_RAD * cos(pim(2) * i + pim(1.5)) * TWOFIFTHS;
            _vertex[i][1] = INS_CIRCLE_RAD * sin(pim(2) * i + pim(1.5)) * TWOFIFTHS;
            _vertex[i][2] = -INS_SPHERE_RAD;
        }
        return &_vertex[0][0];
    }
    //Creates the common starting vertexes for all pieces that are FACES
    double* faceInit() {
        numSides = 0;
        for (int i = 0; i < 5; ++i) {
            //This puts it on the front face. (same as center but larger since not * 2 / 5)
            //_vertex[i][0] = INS_CIRCLE_RAD * cos(pim(2) * i + pim(1.5)); 
            //_vertex[i][1] = INS_CIRCLE_RAD * sin(pim(2) * i + pim(1.5));
            //_vertex[i][2] = -INS_SPHERE_RAD;
            //This puts it on the back face
            _vertex[i][0] = COSPIM35 + EDGEFIFTH * TWOFIFTHS;
            _vertex[i][1] = -SINPIM35;
            _vertex[i][2] = -INS_SPHERE_RAD;
            rotateVertex(_vertex[i], 'z', pim(2));
            rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
            rotateVertex(_vertex[i], 'z', i * pim(2));
        }
        return &_vertex[0][0];
    }
};

static void rotateVertex(double &vx, double &vy, double angle)
{
    const double r = sqrt(vx * vx + vy * vy);
    double a = vy > 0 ? acos(vx / r) : 2 * PI - acos(vx / r);
    a += angle;
    vx = r * cos(a);
    vy = r * sin(a);
}

static void rotateVertex(double *vertex, char axis, double angle)
{
    switch (axis) {
    case 'x':
        rotateVertex(vertex[1], vertex[2], angle);
        break;
    case 'y':
        rotateVertex(vertex[0], vertex[2], angle);
        break;
    case 'z':
        rotateVertex(vertex[0], vertex[1], angle);
        break;
    default:
        break;
    }
}

//main transform: used in almost every other algo
static void axis1multi(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis1, pim(pack.multi));
}
static void CenterSide1(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis1, pim(1));
    rotateVertex(target, pack.axis2, PI - SIDE_ANGLE);
    axis1multi(target, pack);
}
static void CenterCenter(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis1, PI);
}
static void CenterSide2(double* target, piecepack &pack) {
    CenterCenter(target, pack);
    rotateVertex(target, pack.axis2, PI - SIDE_ANGLE);
    rotateVertex(target, 'z', pim(pack.multi));
    //This is always z, because axis1/2 are usually y/x and
    //is re-used by face, where it is Z.
}
static void CornerGrp3(double* target, piecepack &pack) {
    CenterSide1(target, pack);
    rotateVertex(target, pack.axis2, PI);
}
static void CornerGrp4(double* target, piecepack &pack) {
    CenterCenter(target, pack);
    rotateVertex(target, pack.axis2, pim(pack.multi));
}
static void EdgeGrp2(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis1, pim(3));
    rotateVertex(target, pack.axis2, PI - SIDE_ANGLE);
    axis1multi(target, pack);
}
static void EdgeGrp3(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis1, pim(6));
    EdgeGrp2(target, pack);
}
static void EdgeGrp4(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis1, pim(8));
    EdgeGrp2(target, pack);
}
static void EdgeGrp5(double* target, piecepack &pack) {
    pack.multi += 1;
    rotateVertex(target, pack.axis1, pim(2));
    rotateVertex(target, pack.axis2, SIDE_ANGLE);
    axis1multi(target, pack);
}
static void EdgeGrp6(double* target, piecepack &pack) {
    rotateVertex(target, pack.axis2, PI);
    axis1multi(target, pack);
}
