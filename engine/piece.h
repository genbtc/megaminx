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
	BLACK = 0,
    WHITE,
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
	int i;
    double r;
    double g;
    double b;
    const wchar_t* name;
};

//list of the 12 colors in R,G,B from 0.0-1.0(0-255)
static color g_colorRGBs[COLOR_STATES] =
{
	{  0, 0.0, 0.0, 0.0, L"BLACK" },
    {  1, 1.0, 1.0, 1.0, L"WHITE" },
    {  2, 0.0, 0.0, 1.0, L"BLUE" },
    {  3, 1.0, 0.0, 0.0, L"RED" },
    {  4, 0.0, 0.5, 0.0, L"GREEN" },
    {  5, 0.5, 0.0, 1.0, L"PURPLE" },
    {  6, 1.0, 1.0, 0.0, L"YELLOW" },
    {  7, 0.5, 0.5, 0.5, L"GRAY" },
    {  8, 0.2, 0.5, 1.0, L"LIGHT_BLUE"},
    {  9, 1.0, 0.4, 0.1, L"ORANGE" },
    { 10, 0.3, 1.0, 0.3, L"LIGHT_GREEN" },
    { 11, 0.9, 0.4, 1.0, L"PINK" },
    { 12, 1.0, 1.0, 0.8, L"BONE" }
};
struct piecepack
{
    char axis1, axis2;
    int multi;
};
class Piece
{
public:
    //virtual destructor
    virtual ~Piece() {}

    double _vertex[7][3];
	struct _data
	{
		//data-members
		double _color[3][3];
		int _colorNum[3];
		const wchar_t* _colorName[3];
		Vector3d v3d[7];
	} data;
	int numSides;

	//swap
	void swapdata(_data &out)
	{
		const auto temp = data;
		data = out;
		out = temp;
	}
    //getter
    double* getcolor()
    {
	    return &data._color[0][0];
    }
    //setter
    void setColor(int i, color c)
    {
	    data._color[i][0] = c.r;
	    data._color[i][1] = c.g;
	    data._color[i][2] = c.b;
	    data._colorName[i] = c.name;
	    data._colorNum[i] = c.i;
    }
    //common
    void initColorIndex(int idx,int k)
    {
        const auto color = g_colorRGBs[k];
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
	    //set non-existant 3rd side of edge to
	    // 0==black aka not undefined so we can re-use corner.
	    initColorIndex(2, 0);
    }
    //store corner colors
    void initColor(int a, int b, int c)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
        initColorIndex(2, c);
    }
	//check if color-num (int) matches any colors
    // currently stored in struct data (3 sided)
	bool matchesColor(int color) const
	{
		if ((data._colorNum[0] == color) || 
		    (data._colorNum[1] == color) || 
		    (data._colorNum[2] == color))
		{
			return true;
		}
		return false;
	}
    /**
     * \brief 
     * \param target used in almost every other algo
     * \param pack 
     */
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
	    CenterCenter(target, pack);
        rotateVertex(target, pack.axis2, pim(pack.multi));
    }
    static void CornerGrp4(double* target, piecepack &pack)
    {
        CenterSide1(target, pack);
        rotateVertex(target, pack.axis2, PI);
    }
    static void EdgeGrp2(double* target, piecepack &pack)
    {
	    rotateVertex(target, pack.axis1, pim(3));
	    rotateVertex(target, pack.axis2, PI - SIDE_ANGLE);
        axis1multi(target, pack);
    }
    static void EdgeGrp3(double* target, piecepack &pack)
    {
	    rotateVertex(target, pack.axis1, pim(6));
        EdgeGrp2(target, pack);
    }
    static void EdgeGrp4(double* target, piecepack &pack)
    {
	    rotateVertex(target, pack.axis1, pim(8));
        EdgeGrp2(target, pack);
    }
    static void EdgeGrp5(double* target, piecepack &pack)
    {
	    rotateVertex(target, pack.axis1, pim(2));
	    rotateVertex(target, pack.axis2, SIDE_ANGLE);
        axis1multi(target, pack);
    }
    static void EdgeGrp6(double* target, piecepack &pack)
    {
        rotateVertex(target, pack.axis2, PI);
        axis1multi(target, pack);
    }
    static void OpSwitcher(double* target, piecepack &pack, int opnum)
    {
        if (opnum < 1) return;
        switch (opnum) {
        case 1:
            axis1multi(target, pack);
        case 2:
            CenterSide1(target, pack);
        case 3:
            CenterCenter(target, pack);
        case 4:
            CenterSide2(target, pack);
        case 5:
            CornerGrp3(target, pack);
        case 6:
            CornerGrp4(target, pack);
        case 7:
            EdgeGrp2(target, pack);
        case 8:
            EdgeGrp3(target, pack);
        case 9:
            EdgeGrp4(target, pack);
        case 10:
            EdgeGrp5(target, pack);
        case 11:
            EdgeGrp6(target, pack);
        default:
            break;
        }
    }

    /**
	 * \brief flip/rotate/switch-colors for current piece.
	 * \param corner Boolean true if its a corner piece. False if its Edge.
	 * TODO: check what type of piece it is based on other fields.
	 */
	void flip(bool corner)
	{
		double buf[3];
		for (int i = 0; i < 3; ++i) buf[i] = data._color[0][i];
		for (int i = 0; i < 3; ++i) data._color[0][i] = data._color[1][i];
		for (int i = 0; i < 3; ++i) data._color[1][i] = buf[i];
		if (corner)
		{
			for (int i = 0; i < 3; ++i) buf[i] = data._color[1][i];
			for (int i = 0; i < 3; ++i) data._color[1][i] = data._color[2][i];
			for (int i = 0; i < 3; ++i) data._color[2][i] = buf[i];
		}
	}
	/**
	* \brief Does two flips. Thats it.
	*/
	void flipTwice(bool corner)
	{
		flip(corner);
		flip(corner);
	}
	
	double* cacheCorner()
	{
		double* _cacheCorner;
	    const auto temp = &_vertex[0][0];
		cornerInit();
		_cacheCorner = &_vertex[0][0];
		_vertex[0][0] = *temp;
		return _cacheCorner;
	}
	void cornerInit()
	{
		numSides = 3;
		for (int i = 0; i < 7; ++i)
		{
			_vertex[i][2] = -INS_SPHERE_RAD;
		}

		_vertex[0][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) / 5 * 2;
		_vertex[0][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10) / 5 * 2;

		_vertex[1][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) + 100 / sin(2 * PI / 5) * 2 / 5;
		_vertex[1][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10);

		_vertex[2][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10);
		_vertex[2][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10);

		_vertex[3][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) - 100 / sin(2 * PI / 5) * 2 / 5;
		_vertex[3][1] = INS_CIRCLE_RAD * sin(3 * PI / 10);
		rotateVertex(_vertex[3], 'z', PI * 2 / 5);

		_vertex[4][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2;
		_vertex[4][1] = INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2;
		rotateVertex(_vertex[4], 'z', -PI * 3 / 5);
		rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);
		rotateVertex(_vertex[4], 'z', PI * 2 / 5);

		_vertex[5][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) - 100 / sin(2 * PI / 5) * 2 / 5;
		_vertex[5][1] = INS_CIRCLE_RAD * sin(3 * PI / 10);
		rotateVertex(_vertex[5], 'z', -PI * 3 / 5);
		rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
		rotateVertex(_vertex[5], 'z', PI * 2 / 5);

		_vertex[6][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2;
		_vertex[6][1] = INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2;
		rotateVertex(_vertex[6], 'z', -PI * 5 / 5);
		rotateVertex(_vertex[6], 'x', PI - SIDE_ANGLE);
	}
	double* cacheEdge()
	{
		double* _cacheEdge;
		const auto temp = &_vertex[0][0];
		edgeInit();
		_cacheEdge = &_vertex[0][0];
		_vertex[0][0] = *temp;
		return _cacheEdge;
	}
	void edgeInit()
	{
		numSides = 2;
		for (int i = 0; i < 6; ++i)
		{
			_vertex[i][2] = -INS_SPHERE_RAD;
		}

		_vertex[0][0] = 0.99 * (INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) / 5 * 2);
		_vertex[0][1] = 0.99 * (INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10) / 5 * 2);

		_vertex[1][0] = 0.99 * (INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2);
		_vertex[1][1] = 0.99 * (INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2);

		_vertex[2][0] = 0.99 * (INS_CIRCLE_RAD * cos(3 * PI / 10) - 100 / sin(2 * PI / 5) * 2 / 5);
		_vertex[2][1] = 0.99 * (INS_CIRCLE_RAD * sin(3 * PI / 10));

		_vertex[3][0] = 0.99 * (INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) + 100 / sin(2 * PI / 5) * 2 / 5);
		_vertex[3][1] = 0.99 * (INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10));

		_vertex[4][0] = _vertex[1][0];
		_vertex[4][1] = _vertex[1][1];
		rotateVertex(_vertex[4], 'z', PI);
		rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);

		_vertex[5][0] = _vertex[0][0];
		_vertex[5][1] = _vertex[0][1];
		rotateVertex(_vertex[5], 'z', PI);
		rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
	}
};

#endif