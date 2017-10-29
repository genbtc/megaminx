/** @file point_sets.h
 *  @brief declaration of different point sets
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#pragma once

#include "utils_math.h"
#include "material_point.h"

///////////////////////////////////////////////////////////////////////////////
class OscilatorPointSet : public MaterialPointSet
{
public:
	double m_k;
	/** basiclen between points */
	double m_baseLen;
	/** force -= 2.0*resistance*point.vel,    */
	double m_resistance;
	/** force -= 2.0*suppression*dot(velDiff, r)*r    */
	double m_suppression;
	/** calculate elastic forces only when stretching */
	bool m_forceOnStretch;
public:
	OscilatorPointSet(unsigned int count);
	~OscilatorPointSet() { }

	virtual Vec3d Force(unsigned int i);
};

///////////////////////////////////////////////////////////////////////////////
class StiffOscilatorPointSet : public OscilatorPointSet
{
protected:
	std::vector<Vec3d> m_stiffnessForces;
public:
	double m_stiffness;
public:
	StiffOscilatorPointSet(unsigned int count);
	~StiffOscilatorPointSet() { }

	virtual Vec3d Force(unsigned int i);
	virtual void BeforeStep(double deltaTime);
};

///////////////////////////////////////////////////////////////////////////////
class LinePointSet : public StiffOscilatorPointSet
{
public:
	Vec3d m_gForce;
public:
	LinePointSet(unsigned int count);
	~LinePointSet() { }

	void Reset(double basicLen, const Vec3d &start, const Vec3d &dir);

	virtual Vec3d Force(unsigned int i);
};



