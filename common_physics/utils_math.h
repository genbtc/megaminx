/** @file utils_math.h
 *  @brief math utilities
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#pragma once
#include <cstdlib>
#include <cstring>
#include <cmath>

inline double DegToRad(double a) { return a*0.01745329252; };  /**< ang * M_PI / 180.0f */
inline double RadToDeg(double a) { return a*57.29577951; };    /**< rads * 180.0f / M_PI */

//////////////////////////////////////////////////////////////////////////
// Vector3
template <typename T>
class TVector3
{
public:
	union
	{
		T m[3];
		
		struct
		{
			T x, y, z;
		};
	};
public:
	TVector3() { }
	TVector3(const T& a): x(a), y(a), z(a) { }
	TVector3(const double* a) : x(a[0]) , y(a[1]) , z(a[2]) {}
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
};

typedef TVector3<float> Vec3f;
typedef TVector3<double> Vec3d;
typedef TVector3<short> Vec3i;

//////////////////////////////////////////////////////////////////////////
// Matrix3d
class  Matrix3d
{
public:
	union
	{
		double M[9];

		struct
		{
			double M00, M01, M02;
			double M10, M11, M12;	        
			double M20, M21, M22;
		};
	};
public:
	Matrix3d() { }
	Matrix3d(const Matrix3d& mt)							{ memcpy((void *)M, (const void *)mt.M, sizeof(M)); }
	Matrix3d(double *v)										{ memcpy((void *)M, (const void *)v, sizeof(M)); }
	Matrix3d(double m00, double m01, double m02, 
			  double m10, double m11, double m12,
			  double m20, double m21, double m22)	{ M00 = m00; M01 = m01; M02 = m02; 
													  M10 = m10; M11 = m11; M12 = m12; 
													  M20 = m20; M21 = m21; M22 = m22;  }
	~Matrix3d() { }

	Matrix3d operator* (const Matrix3d& mt) const;
	Matrix3d operator* (double v) const;
	Matrix3d operator+ (const Matrix3d& mt) const;

	void operator *= (const Matrix3d& mt);
	void operator *= (double v);
	void operator += (const Matrix3d& mt);

	Matrix3d& operator=(const Matrix3d& right) {
		if(this == &right) return *this;
		memcpy((void *)M, (const void *)right.M, sizeof(double) * 9);
		return *this;
	}

    // casting to a double array
	operator double* ()		 { return (double *)M; }

	/** covert to matrix that can be passed to OpenGL */
	double *ToMatrix4x4(double *out) 
	{
		out[0]  = M[0];  out[1] = M[1];  out[2] = M[2];  out[3] = 0.0;
		out[4]  = M[3];  out[5] = M[4];  out[6] = M[5];  out[7] = 0.0;
		out[8]  = M[6];  out[9] = M[7];  out[10] = M[8]; out[11] = 0.0;
		out[12] = 0.0;   out[13] = 0.0;  out[14] = 0.0; out[15] = 1.0;
		return out;
	}

	static const double IDENTITY[9];
};

//////////////////////////////////////////////////////////////////////////
// matrix functions 
inline Matrix3d StarOperator(const Vec3d& v);
Vec3d TransformVec3(const Matrix3d& mat, const Vec3d& v);
inline void Zero(Matrix3d* mt);
inline void Identity(Matrix3d* mt);
void Transpose(const Matrix3d &mt, Matrix3d* out);
int Inverse(const Matrix3d &mt, Matrix3d* out);
Matrix3d RotationX(double ang);
Matrix3d RotationY(double ang);
Matrix3d RotationZ(double ang);
inline double Det(const Matrix3d& mt);

//////////////////////////////////////////////////////////////////////////
// Quaterniond
class  Quaterniond
{
public:
	union
	{
		double m[4];
		struct
		{
			double w, x, y, z;
		};
	};
public:
	Quaterniond() { }
	Quaterniond(double a, double b, double c, double d) { w = a; x = b; y = c; z = d; }
	Quaterniond(double a, const Vec3d & v) { w = a; x = v.x; y = v.y; z = v.z; }
	Quaterniond(const Quaterniond& q) { w = q.w; x = q.x; y = q.y; z = q.z; }
	~Quaterniond() { }

	Quaterniond operator+ (const Quaterniond& q) { return Quaterniond(w+q.w, x+q.x, y+q.y, z+q.z); }
	Quaterniond operator- (const Quaterniond& q) { return Quaterniond(w-q.w, x-q.x, y-q.y, z-q.z); }
	Quaterniond operator* (double v) { return Quaterniond(w*v, x*v, y*v, z*v); }
	Quaterniond operator* (const Quaterniond& q);

	void operator+= (const Quaterniond& q) { w+=q.w; x+=q.x; y+=q.y; z+=q.z; }
	void operator-= (const Quaterniond& q) { w-=q.w; x-=q.x; y-=q.y; z-=q.z; }
	void operator*= (double v) { w*=v; x*=v; y*=v; z*=v; }
	void operator*= (const Quaterniond& q);

	Matrix3d *ToMatrix3x3(Matrix3d *out);
	double *ToMatrix4x4(double *out);
};

//////////////////////////////////////////////////////////////////////////
// quaternion functions 
inline double Norm(const Quaterniond* q)								{ return (sqrt(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z)); }
inline double NormSquare(const Quaterniond* q)							{ return ((q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z)); }
void Normalize(Quaterniond* q);
inline Quaterniond Conjugate(const Quaterniond* q)					{ return Quaterniond(q->w, -q->x, -q->y, -q->z); }
Quaterniond Inverse(const Quaterniond* q);
Quaterniond AxisRotationToQuaternion(const Vec3d* axis, double rot);

//////////////////////////////////////////////////////////////////////////
// other useful functions
//////////////////////////////////////////////////////////////////////////
inline Vec3d ClosestPoint(const Vec3d &A, const Vec3d &B, const Vec3d &P, double *t)
{
	Vec3d AB         = B - A;
	double ab_square = DotProduct(AB, AB);
	Vec3d AP         = P - A;
	double ap_dot_ab = DotProduct(AP, AB);

	*t  = ap_dot_ab / ab_square; 

	// calculate the closest point
	Vec3d Q = A + AB * (*t);

	return Q;
}

//////////////////////////////////////////////////////////////////////////
// inline functions:

template <typename T> T Rand(const T &a, const T &b) { return ((T)rand() / RAND_MAX) * (b - a) + a; }

//////////////////////////////////////////////////////////////////////////
inline Matrix3d StarOperator(const Vec3d& v) 
{ 
	return Matrix3d(0,   v.z, -v.y,
					-v.z,0,   v.x,
					v.y, -v.x, 0.0); 
}

//////////////////////////////////////////////////////////////////////////
inline void Zero(Matrix3d* mt)
{
	memset((void *)mt->M, 0, 9*sizeof(double)); 
}

//////////////////////////////////////////////////////////////////////////
inline void Identity(Matrix3d* mt)	
{ 
	memcpy((void *)mt->M, (const void *)Matrix3d::IDENTITY, 9*sizeof(double)); 
}

//////////////////////////////////////////////////////////////////////////
inline double Det(const Matrix3d& mt)
{
	return (mt.M00*(mt.M11*mt.M22 - mt.M21*mt.M12) - 
			mt.M01*(mt.M10*mt.M22 - mt.M20*mt.M12) + 
			mt.M02*(mt.M10*mt.M21 - mt.M20*mt.M11) );
}