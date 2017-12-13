/** @file utils_math.h
 *  @brief math utilities
 *
 *  @author Bartlomiej Filipek
 *  @date March 2011
 *  //Edited by genBTC 2017 for megaminx
 */

#pragma once
#include <cmath>
#include <cstdlib>
#include <cstring>

inline double DegToRad(double a) { return a*0.01745329252; };  /**< ang * M_PI / 180.0f */
inline double RadToDeg(double a) { return a*57.29577951; };    /**< rads * 180.0f / M_PI */

//////////////////////////////////////////////////////////////////////////
// Vector3
template <typename T>
class TVector3 {
public:
    union {
        T m[3];

        struct {
            T x, y, z;
        };
    };
public:
    TVector3() { }
    TVector3(const T& a): x(a), y(a), z(a) { }
    TVector3(const double* a) : x(a[0]), y(a[1]), z(a[2]) {}
    TVector3(const T& a, const T& b, const T& c): x(a), y(b), z(c) { }
    TVector3(const TVector3<T> & a) { x = a.x; y = a.y; z = a.z; }

    void operator += (const TVector3<T> & a) { x += a.x; y += a.y; z += a.z; }
    void operator -= (const TVector3<T> & a) { x -= a.x; y -= a.y; z -= a.z; }
    void operator *= (const T &v) { x *= v; y *= v; z *= v; }

    friend TVector3<T> operator+ (const TVector3<T> & a, const TVector3<T> & b) { return TVector3<T>(a.x+b.x, a.y+b.y, a.z+b.z); }
    friend TVector3<T> operator- (const TVector3<T> & a, const TVector3<T> & b) { return TVector3<T>(a.x-b.x, a.y-b.y, a.z-b.z); }
    friend TVector3<T> operator- (const TVector3<T> & a) { return TVector3<T>(-a.x, -a.y, -a.z); }

    friend TVector3<T> operator* (const TVector3<T> & a, const T &v) { return TVector3<T>(a.x*v, a.y*v, a.z*v); }
    friend TVector3<T> operator* (const T &v, const TVector3<T> & a) { return TVector3<T>(a.x*v, a.y*v, a.z*v); }

    friend TVector3<T> operator/ (const TVector3<T> & a, const T &v) { return TVector3<T>(a.x/v, a.y/v, a.z/v); }

    friend T Distance(const TVector3<T> & a, const TVector3<T> & b) { TVector3<T> d = a-b; return d.Length(); }
    friend T DotProduct(const TVector3<T> & a, const TVector3<T> & b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
    friend TVector3<T> CrossProduct(const TVector3<T> &a, const TVector3<T> &b) { return TVector3<T>(a.y*b.z - a.z*b.y, -(a.x*b.z - a.z*b.x), a.x*b.y - a.y*b.x); }

    T Length() const { return sqrt(x*x + y*y + z*z); }
    void Normalize() { T f = 1.0/Length(); x *= f; y *= f; z *=f; }
    TVector3<T> ClosestPoint(const TVector3<T> &A, const TVector3<T> &B, const TVector3<T> &P, double *t) 
    {
        TVector3<T>  AB = B - A;
        double ab_square = DotProduct(AB, AB);
        TVector3<T>  AP = P - A;
        double ap_dot_ab = DotProduct(AP, AB);
        *t = ap_dot_ab / ab_square;
        TVector3<T> ClosestPoint = A + AB * (*t);
        return ClosestPoint;
    }
};

typedef TVector3<float> Vec3f;
typedef TVector3<double> Vec3d;
typedef TVector3<short> Vec3i;
