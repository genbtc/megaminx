/** @file mesh_point_sets.h
 *  @brief declaration of different "2D" point sets
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#pragma once

#include <assert.h>
#include "utils_math.h"
#include "material_point.h"
#include "prohibited_area.h"

class MeshPointSet : public MaterialPointSetWithProhibitedArea
{
public:
	double m_k; 	
	double m_t;
	double m_tt;
	double m_s;
	double m_lx,m_ly; 
	int m_Nx, m_Ny;
	bool m_forceOnStretch;
	Vec3d *m_stiffnessForces;
	Vec3d g; //przyspieszenie ziemskie
public:
	MeshPointSet(unsigned int nx, unsigned ny);
	~MeshPointSet() { delete [] m_stiffnessForces; }

	void Create(double lx, double ly);
	MaterialPoint *Point2D(unsigned int x, unsigned int y) { assert(x < m_Nx && y < m_Ny); return &m_points[x*m_Nx + y]; }
	void SetBound(unsigned int x, unsigned int y, bool b) { assert(x < m_Nx && y < m_Ny); m_bounds[x*m_Nx + y] = b; }

	virtual Vec3d Force(unsigned int i);
	virtual void BeforeStep(double deltaTime);
};





