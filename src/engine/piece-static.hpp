#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <cstring>

using std::acos;
using std::atan;
using std::cos;
using std::sin;
using std::sqrt;

//default size in 3D coords for main megaminx
static const long double DODESIZE = 100;
//common geometric constants
static const long double PI = acos(-1.);           //3.1415927410125732
//Golden Ratio (Phi) (also The ratio between the side length of a regular pentagon and one of its diagonals.)
static const long double PHI = (1 + sqrt(5.)) / 2;  //1.6180340051651001
static const long double SIDE_ANGLE = 2 * atan(PHI); //2.0344439448698051
//inscribed sphere radius ( ri = a / 2 * √ ( 25 + 11 * √5 ) / 10 )
static const long double INS_SPHERE_RAD = DODESIZE * sqrt(10+22 / sqrt(5.)) / 4;   //111.35163307189941 
static const long double INS_CIRCLE_RAD = DODESIZE / sqrt((5 - sqrt(5.)) / 2);     // 85.065082037033278
#define pim(x) x*PI/5
//megaminx vertex math shortcuts
static const long double TWOFIFTHS = 2./5;
static const long double EDGEFIFTH = DODESIZE / sin(pim(2));            //105.14622122913930
static const long double COSPIM35 = INS_CIRCLE_RAD * cos(pim(3.5));     //-50.000004917867173
static const long double COSPIM15 = INS_CIRCLE_RAD * cos(pim(1.5));     //49.999998901510480
static const long double SINPIM35 = INS_CIRCLE_RAD * sin(pim(3.5));     //68.819093936061520

struct piecepack {
    char axis1, axis2;
    int multi;
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


// Function to rotate array
void arrayRotateRightTemp(int arr[], int d, int n)
{
    // Storing rotated version of array
    int temp[n];
    // Keeping track of the current index of temp[]
    int k = 0;
 
    // Storing the n - d elements of array arr[] to the front of temp[]
    for (int i = d; i < n; i++) {
        temp[k] = arr[i];
        k++;
    }
 
    // Storing the first d elements of array arr[] into temp
    for (int i = 0; i < d; i++) {
        temp[k] = arr[i];
        k++;
    }
 
    // Copying the elements of temp[] in arr[] to get the final rotated array
    for (int i = 0; i < n; i++) {
        arr[i] = temp[i];
    }
}