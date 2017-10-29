/** @file material_point.h
 *  @brief declaration of material point and material poin set used in physics simulation
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#pragma once
#include <vector>
#include "utils_math.h"

///////////////////////////////////////////////////////////////////////////////
/** most of fields, for simplicity, are public - this migh couse some errors when
  * used without care */
class MaterialPoint
{
public:
	enum AlgorithmType { atEuler, atVerlet };
	static AlgorithmType s_algType;
public:
	/** next position calculated by the algorithm */
	Vec3d m_nextPos;
	/** this pos will be used to render point */
	Vec3d m_pos;
	/** position from step back */
	Vec3d m_prevPos;

	Vec3d m_nextVel;
	Vec3d m_vel;

	Vec3d m_acceleration;

	/** by default it is set to 1.0 */
	double m_mass;
	/** by default it is set to 0 */
	double m_radius;

	unsigned int m_stepCounter;
public:
	MaterialPoint();
	~MaterialPoint();

	/** calculates next step of simulation - calculates "next" params */
	void PrepareMove(double deltaTime, const Vec3d &force);
	void PrepareMoveEuler(double deltaTime, const Vec3d &force);
	/** updates simulation */
	void UpdateMove();
	
	/** important method if you want to reset whole simulation */
	void Reset() { m_nextPos = m_pos; m_prevPos = m_pos; m_nextVel = m_vel; m_stepCounter = 0; }

	/** retusn true if point (point with small radius) collides with the ray */
	bool RayTest(const Vec3d &start, const Vec3d &end, Vec3d *pt, double *t, double epsilon = 0.0001);
protected:
	void CalcEuler(double delta);
	void CalcVerlet(double delta);
};

///////////////////////////////////////////////////////////////////////////////
class MaterialPointSet
{
protected:
	std::vector<MaterialPoint> m_points;
	/** if true then smulation is not performed for given point, 
	 *  by default it is false for every point */
	std::vector<bool> m_bounds;
	unsigned int m_count;
public:
	MaterialPointSet(unsigned int count);
	virtual ~MaterialPointSet() { }

	virtual Vec3d Force(unsigned int id) = 0;
	virtual void BeforeStep(double deltaTime) { };
	virtual void AfterStep(double deltaTime)  { };
	virtual void Update(double deltaTime);

	/** test ray collision against points, returns true when found collision and
	 * in "id" there is id of the point (point wth rad represents little sphere) that collides with the ray.
	 * if more than one cllision is found then "id" points to the closest to "start" point */
	bool RayTest(const Vec3d &start, const Vec3d &end, unsigned int *id, double *t, double epsilon = 0.0001);
public:
	unsigned int PointCount() const { return m_count; }
	MaterialPoint *Point(unsigned int i) { return &m_points[i]; } 
	void SetBound(unsigned int id, bool bound) { m_bounds[id] = bound; }
};