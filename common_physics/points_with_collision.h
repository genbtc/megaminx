/** @file points_with_collision.h
 *  @brief declaration of material point sets that collide with each others
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#pragma once

#include <assert.h>
#include "utils_math.h"
#include "material_point.h"
#include "prohibited_area.h"

///////////////////////////////////////////////////////////////////////////////
class PointSetWithCollisions : public MaterialPointSetWithProhibitedArea
{
public:
	double m_epsilon;
public:
		
public:
	PointSetWithCollisions(int count);
	~PointSetWithCollisions() { }

	virtual void CheckCollisions(double deltaTime);	

	virtual void AfterStep(double deltaTime);

	// we assume that external force is 0, but one can change it to gravity for instance.
	virtual Vec3d Force(unsigned int id) { return Vec3d(0.0); }
};

