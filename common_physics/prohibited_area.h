/** @file prohibited_area.h
 *  @brief declaration of prohibited areas, used for simple collisions
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#pragma once

#include <assert.h>
#include "utils_math.h"
#include "material_point.h"

class ProhibitedArea
{
public:
	double m_bounceFactor;
	double m_frictionFactor;
public:
	ProhibitedArea() { m_bounceFactor = 0.0; m_frictionFactor = 0.0; }
	ProhibitedArea(double bounceFactor, double frictionFactor) : m_bounceFactor(bounceFactor), m_frictionFactor(frictionFactor) { }
	virtual ~ProhibitedArea() { }

	virtual bool IsInArea(Vec3d &pos, double margin, Vec3d *normal) = 0;
};

class FloorArea : public ProhibitedArea
{
public:
	/** y pos of the floor */
	double m_level;
public:
	FloorArea(double level) : m_level(level) { }
	FloorArea(double level, double bounceF, double frictionF) : ProhibitedArea(bounceF, frictionF), m_level(level) { }

	virtual bool IsInArea(Vec3d &pos, double margin, Vec3d *normal);
};

class PlaneArea : public ProhibitedArea
{
public:
	Vec3d m_normal;
	Vec3d m_point;
public:
	PlaneArea(const Vec3d &n, const Vec3d &p) : m_normal(n), m_point(p) { }
	PlaneArea(const Vec3d &n, const Vec3d &p, double bounceF, double frictionF) : ProhibitedArea(bounceF, frictionF), m_normal(n), m_point(p) { }

	virtual bool IsInArea(Vec3d &pos, double margin, Vec3d *normal);
};

class SphereArea : public ProhibitedArea
{
public:
	double m_radius;
	Vec3d m_center;
public:
	SphereArea() { }
	SphereArea(double r, const Vec3d &p) : m_radius(r), m_center(p) { }
	SphereArea(double r, const Vec3d &p, double bounceF, double frictionF) : ProhibitedArea(bounceF, frictionF), m_radius(r), m_center(p) { }

	virtual bool IsInArea(Vec3d &pos, double margin, Vec3d *normal);
};

class InsideBoxArea : public ProhibitedArea
{
public:
	Vec3d m_min;
	Vec3d m_max;
public:
	InsideBoxArea() { }
	InsideBoxArea(const Vec3d &a, const Vec3d &b) : m_min(a), m_max(b) { }
	InsideBoxArea(const Vec3d &a, const Vec3d &b, double bounceF, double frictionF) : ProhibitedArea(bounceF, frictionF), m_min(a), m_max(b) { }

	virtual bool IsInArea(Vec3d &pos, double margin, Vec3d *normal);

	double SizeX() { return m_max.x - m_min.x; }
	double SizeY() { return m_max.y - m_min.y; }
	double SizeZ() { return m_max.z - m_min.z; }
	double CenterX() { return (m_max.x + m_min.x)*0.5; }
	double CenterY() { return (m_max.y + m_min.y)*0.5; }
	double CenterZ() { return (m_max.z + m_min.z)*0.5; }
};

class ComplexArea : public ProhibitedArea
{
protected:
	std::vector<ProhibitedArea *> m_areas;
public:
	ComplexArea(double bounceF, double frictionF) : ProhibitedArea(bounceF, frictionF) { }
	virtual ~ComplexArea() { }

	void Add(ProhibitedArea *area, bool setFactors = true) { if (setFactors) { area->m_bounceFactor = m_bounceFactor; } area->m_frictionFactor = m_frictionFactor; m_areas.push_back(area); }
	int SubAreasCount() const { return (int)m_areas.size(); }

	virtual bool IsInArea(Vec3d &pos, double margin, Vec3d *normal);
};

///////////////////////////////////////////////////////////////////////////////
class MaterialPointSetWithProhibitedArea : public MaterialPointSet
{
public:
	ProhibitedArea *m_prohibitedArea;

public:
	MaterialPointSetWithProhibitedArea(int count) : MaterialPointSet(count), m_prohibitedArea(NULL) { }
	MaterialPointSetWithProhibitedArea(int count, ProhibitedArea *area) : MaterialPointSet(count), m_prohibitedArea(area) { }
	virtual ~MaterialPointSetWithProhibitedArea() { }

	virtual void Update(double deltaTime);
protected:
	void UpdateOnCollision(int id, double deltaTime);
};