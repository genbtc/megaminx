/** @file collision_shapes.h
 *  @brief declaration of collision shapes that can be used in physics simulation
 *
 *	@author Bartlomiej Filipek
 *	@date May 2011
 */

#pragma once

#include <assert.h>
#include "utils_math.h"

//////////////////////////////////////////////////////////////////////////
// BoundingSphere
class BoundingSphere
{
public:
	Vec3d m_center;
	double m_radius;
public:
	BoundingSphere() { }
	BoundingSphere(const Vec3d &c, double r): m_center(c), m_radius(r) { }
};

// bounding sphere functions:
bool TestCollision(const BoundingSphere &a, const BoundingSphere &b);

//////////////////////////////////////////////////////////////////////////
// AABB - Axis Aligned Bounding Box
class AABB
{
public:
	Vec3d m_min;
	Vec3d m_max;
public:
	AABB() { }
	AABB(const Vec3d &m, const Vec3d& x): m_min(m), m_max(x) { }

	void Reset();
	void AddPoint(const Vec3d &pt);
};

// AABB functions:
bool TestCollision(const AABB &a, const AABB &b);

//////////////////////////////////////////////////////////////////////////
// OBB - Oriented Bounding Box
class OBB
{
public:
	Vec3d m_points[8];
public:
	OBB() { }
	OBB(double half_w, double half_h, double half_d);

	void Rotate(const Matrix3d& rot);
	void Translate(const Vec3d &t);
};

// OBB functions:
bool TestCollision(const OBB &a, const OBB &b);