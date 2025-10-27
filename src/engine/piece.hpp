#pragma once
#include "piece-static.hpp"
#include "piece-color.hpp"

class Piece {
public:
    //virtual destructor
    virtual ~Piece() = default;

    //Coords for GL vertex (up to 7, not all used).
    double _vertex[7][3] = {{0}};
    //Keeps the default number in the piece. do not swap.
    int _defaultPieceNum;
    //Center has 1, Edge has 2, Corner has 3
    int numSides;

    //data-members we can swap out
    struct _data {
        double _color[3][3];
        int _colorNum[3];
        const char* _colorName[3];
        int pieceNum;
        int flipStatus;
        //Highlight in bright green while algorithming (called in .render())
        bool hotPieceMoving = false;
    } data = {};

    //Swaps current data with the &out_param
    void swapdata(_data &out) {
        if (&(this->data) == &out)
            return;
        const _data temp = data;
        data = out;
        out = temp;
    }
    //Vertices:
    //getter
    double* getVertexData() {
        return &_vertex[0][0];
    }
    //Colors:
    //getter
    [[deprecated]] [[maybe_unused]]
    double* getColorData() {
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
    [[deprecated]] [[maybe_unused]]
    void initColor(int a, int b) {
        initColorIndex(0, a);
        initColorIndex(1, b);
        //set non-existant 3rd side of edge to 0=Black
        // aka not undefined so we can re-use corner.
        initColorIndex(2, 0);
        numSides = 2;
    }
    //store Corner colors
    [[deprecated]] [[maybe_unused]]
    void initColor(int a, int b, int c) {
        initColorIndex(0, a);
        initColorIndex(1, b);
        initColorIndex(2, c);
        numSides = 3;
    }
    //Uses the two arrays g_cornerPiecesColors and g_edgePiecesColors to populate piece.
    void initColor(colorpiece color, bool corner=false) {
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

    //Function to Reverse the c-Array.
    template <typename T>
    void arrayReverse(T arr[], int n) {
        for(int i = i; i<n/2; ++i) {
            int temp = arr[i];
            arr[i] = arr[n-i-1];
            arr[n-i-1] = temp;
        }
    }

    //Function to Left rotate a C-array by r positions, n length
    template <typename T>
    void leftRotate(T arr[], int r, int n)
    {
        for (int j = 0; j < r; j++) {
            /* Store first element of array */
            T first = arr[0];
            /* Move each array element to its LEFT */
            for (int i = 0; i < n - 1; i++)
                arr[i] = arr[i + 1];
            /* Copy first element of array to last */
            arr[n - 1] = first;
        }
    }

    //Function to Right rotate a C-array by r positions, n length
    template <typename T>
    void rightRotate(T arr[], int r, int n)
    {
        const int rotations = r % n;
        for(int j = 0; j < rotations; j++) {
            /* Store last element of array */
            T last = arr[n - 1];
            /* Move each array element to its RIGHT */
            for(int i = n - 1; i > 0; i--)
                arr[i] = arr[i - 1];
            /* Copy last element of array to first */
            arr[0] = last;
        }
    }

    //Changes colors. Flip/rotate/switches colors for current piece.
    void flip() {
        leftRotate<double>(data._color[0], 3, numSides * 3);
        leftRotate<int>(data._colorNum, 1, numSides);
        leftRotate<const char*>(data._colorName, 1, numSides);
        const bool isCorner = (numSides == 3);
        (( isCorner && data.flipStatus < 2) ||
         (!isCorner && data.flipStatus == 0))
            ? data.flipStatus++
            : data.flipStatus = 0;
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
