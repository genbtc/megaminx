#pragma once
#include <cmath>
#include "vector3d.h"
#include "color.h"

void rotateVertex(double *vertex, char axis, double angle);
//common constants
static const long double FI = (1 + std::sqrt(5.f)) / 2;
static const long double PI = std::acos(-1);
static const long double SIDE_ANGLE = 2 * std::atan(FI);
static const long double INS_SPHERE_RAD = 100 * std::sqrt(10+22 / std::sqrt(5.f)) / 4;
static const long double INS_CIRCLE_RAD = 100 / std::sqrt((5 - std::sqrt(5.f)) / 2);
#define pim(x) x*PI/5

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

    //Coords for GL vertex (up to 7, not all used)
    double _vertex[7][3];

    //data-members we can swap out
    struct _data
    {
        double _color[3][3];
        int _colorNum[3];
        const wchar_t* _colorName[3];
        Vector3d v3d[7];
    } data;

    //Center has 1, Edge has 2, Corner has 3
    int numSides;

    //Swaps current data with the &out_param
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
    void setColor(int i, colorpack c)
    {
        data._color[i][0] = c.r;
        data._color[i][1] = c.g;
        data._color[i][2] = c.b;
        data._colorName[i] = c.name;
        data._colorNum[i] = c.i;
    }
    //interface function for setter
    void initColorIndex(int idx, int k)
    {
        const colorpack thecolor = g_colorRGBs[k];
        setColor(idx, thecolor);
    }
    //store Center color
    void initColor(int a)
    {
        initColorIndex(0, a);
        numSides = 1;
    }
    //store Edge colors
    void initColor(int a, int b)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
        //set non-existant 3rd side of edge to
        // 0==black aka not undefined so we can re-use corner.
        initColorIndex(2, 0);
        numSides = 2;
    }
    //store Corner colors
    void initColor(int a, int b, int c)
    {
        initColorIndex(0, a);
        initColorIndex(1, b);
        initColorIndex(2, c);
        numSides = 3;
    }
    //Use the two arrays g_cornerPiecesColors and g_edgePiecesColors to populate.
    void initColor(colorpiece color,int corner=0)
    {
        initColorIndex(0, color.a);
        initColorIndex(1, color.b);
        if (corner)
        {
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
	bool matchesColor(int color) const
	{
		return  data._colorNum[0] == color || 
		        data._colorNum[1] == color || 
		        data._colorNum[2] == color;
	}
    //main transform: used in almost every other algo
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
	 * \brief Deals with colors. Flip/rotate/switches colors for current piece.
	 */
	void flip()
	{
	    const bool isCorner = numSides == 3;
		double buf[3];
		for (int i = 0; i < 3; ++i) buf[i] = data._color[0][i];
		for (int i = 0; i < 3; ++i) data._color[0][i] = data._color[1][i];
		for (int i = 0; i < 3; ++i) data._color[1][i] = buf[i];
		if (isCorner)
		{
			for (int i = 0; i < 3; ++i) buf[i] = data._color[1][i];
			for (int i = 0; i < 3; ++i) data._color[1][i] = data._color[2][i];
			for (int i = 0; i < 3; ++i) data._color[2][i] = buf[i];
		}
	    const wchar_t *namebuf;
		namebuf = data._colorName[0];
		data._colorName[0] = data._colorName[1];
		data._colorName[1] = namebuf;
		if (isCorner)
		{
			namebuf = data._colorName[1];
			data._colorName[1] = data._colorName[2];
			data._colorName[2] = namebuf;
		}
		int numbuff;
		numbuff = data._colorNum[0];
		data._colorNum[0] = data._colorNum[1];
		data._colorNum[1] = numbuff;
		if (isCorner)
		{
			numbuff = data._colorNum[1];
			data._colorNum[1] = data._colorNum[2];
			data._colorNum[2] = numbuff;
		}		    
	}

	/**
	* \brief Does two flips. Thats it.
	*/
	void flipTwice()
	{
		flip();
		flip();
	}
    //Creates the common starting vertexes for all pieces that are CORNERS
    double* cornerInit()
    {
        using namespace std;
        numSides = 3;
		for (int i = 0; i < 7; ++i)
		{
			_vertex[i][2] = -INS_SPHERE_RAD;
		}

        _vertex[0][0] = INS_CIRCLE_RAD * cos(pim(3.5)) * 2 / 5;
        _vertex[0][1] = INS_CIRCLE_RAD * sin(pim(3.5)) * 2 / 5;

        _vertex[1][0] = INS_CIRCLE_RAD * cos(pim(3.5)) + 100 / sin(pim(2)) * 2 / 5;
        _vertex[1][1] = INS_CIRCLE_RAD * sin(pim(3.5));

        _vertex[2][0] = INS_CIRCLE_RAD * cos(pim(3.5));
        _vertex[2][1] = INS_CIRCLE_RAD * sin(pim(3.5));

        _vertex[3][0] = INS_CIRCLE_RAD * cos(pim(1.5)) - 100 / sin(pim(2)) * 2 / 5;
        _vertex[3][1] = INS_CIRCLE_RAD * sin(pim(1.5));
        rotateVertex(_vertex[3], 'z', pim(2));

        _vertex[4][0] = INS_CIRCLE_RAD * cos(pim(1.5)) * 2 / 5;
        _vertex[4][1] = INS_CIRCLE_RAD * sin(pim(1.5)) * 2 / 5;
        rotateVertex(_vertex[4], 'z', pim(-3));
        rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);
        rotateVertex(_vertex[4], 'z', pim(2));

        _vertex[5][0] = INS_CIRCLE_RAD * cos(pim(1.5)) - 100 / sin(pim(2)) * 2 / 5;
        _vertex[5][1] = INS_CIRCLE_RAD * sin(pim(1.5));
        rotateVertex(_vertex[5], 'z', pim(-3));
        rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
        rotateVertex(_vertex[5], 'z', pim(2));

        _vertex[6][0] = INS_CIRCLE_RAD * cos(pim(1.5)) * 2 / 5;
        _vertex[6][1] = INS_CIRCLE_RAD * sin(pim(1.5)) * 2 / 5;
        rotateVertex(_vertex[6], 'z', pim(-5));
        rotateVertex(_vertex[6], 'x', PI - SIDE_ANGLE);
		return &_vertex[0][0];
	}
	//Creates the common starting vertexes for all pieces that are EDGES
    double* edgeInit()
	{
        using namespace std;
		numSides = 2;
		for (int i = 0; i < 6; ++i)
		{
			_vertex[i][2] = -INS_SPHERE_RAD;
		}

		_vertex[0][0] = INS_CIRCLE_RAD * cos(pim(3.5)) * 2 / 5;
		_vertex[0][1] = INS_CIRCLE_RAD * sin(pim(3.5)) * 2 / 5;
                        
		_vertex[1][0] = INS_CIRCLE_RAD * cos(pim(1.5)) * 2 / 5;
		_vertex[1][1] = INS_CIRCLE_RAD * sin(pim(1.5)) * 2 / 5;
                        
		_vertex[2][0] = INS_CIRCLE_RAD * cos(pim(1.5)) - 100 / sin(pim(2)) * 2 / 5;
		_vertex[2][1] = INS_CIRCLE_RAD * sin(pim(1.5));
                        
		_vertex[3][0] = INS_CIRCLE_RAD * cos(pim(3.5)) + 100 / sin(pim(2)) * 2 / 5;
		_vertex[3][1] = INS_CIRCLE_RAD * sin(pim(3.5));

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
	double* centerInit()
	{
        using namespace std;
		numSides = 1;
		for (int i = 0; i < 5; ++i)
		{
			_vertex[i][0] = INS_CIRCLE_RAD * cos(pim(2) * i + pim(1.5)) * 2 / 5;
			_vertex[i][1] = INS_CIRCLE_RAD * sin(pim(2) * i + pim(1.5)) * 2 / 5;
			_vertex[i][2] = -INS_SPHERE_RAD;
		}
		return &_vertex[0][0];
	}
	//Creates the common starting vertexes for all pieces that are FACES
	double* faceInit()
	{
        using namespace std;
		numSides = 0;
		for (int i = 0; i < 5; ++i)
		{
			_vertex[i][0] = -INS_CIRCLE_RAD * cos(pim(1.5)) + 100 / sin(pim(2)) * 2 / 5;
			_vertex[i][1] = -INS_CIRCLE_RAD * sin(pim(1.5));
			_vertex[i][2] = -INS_SPHERE_RAD;
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 2 * i * PI / 5);
		}
		return &_vertex[0][0];
	}
};
