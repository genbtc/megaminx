/** @file rigid_body.h
 *  @brief declaration of rigid bodies basic data structures
 *
 *	@author Bartlomiej Filipek
 *	@date May 2011
 */

#pragma once

#include <assert.h>
#include "utils_math.h"
#include "material_point.h"
#include "collision_shapes.h"

#define USE_QUATERNIONS_FOR_ROTATIONS

///////////////////////////////////////////////////////////////////////////////
/** most of fields, for simplicity, are public - this migh couse some errors when
  * used without care */
class RigidBody
{
public: 
	MaterialPoint m_massCenter;
		
#ifdef USE_QUATERNIONS_FOR_ROTATIONS
	Quaterniond m_rotation;
	Quaterniond m_prevRotation;
	Quaterniond m_nextRotation;
#else
	Matrix3d m_rotation;
	Matrix3d m_prevRotation;
	Matrix3d m_nextRotation;
#endif
		
	Vec3d m_angularVel;
	Vec3d m_nextAngularVel;
		
	Matrix3d m_momentOfInertiaTensor;
	Matrix3d m_momentOfInertiaTensorInv;	
		
	int m_stepCounter;
public:
	RigidBody(Vec3d pos, Vec3d vel, Vec3d angVel, Matrix3d momentOfInertiaTensor);
	RigidBody();
	~RigidBody();	
		
	void PrepareRotationEuler(const Vec3d& momentOfForce, double deltaTime); 	
	void PrepareMove(const Vec3d& force, const Vec3d& momentOfForce, double deltaTime); 
	virtual void UpdateMove(); 

	/** can be directly passed to OpenGL */
	double *GetRotationMatrix4x4(double *m) { return m_rotation.ToMatrix4x4(m); }
};

///////////////////////////////////////////////////////////////////////////////
/** RigidBody: Box */
class Box : public RigidBody
{
public:
	double m_width, m_height, m_length;
	double m_density;
	/** mass of the whole object, calculated in Mass method, one should recalculate mass when any param changes! */
	double m_mass;

	Vec3d m_points[8];
	Vec3d m_pointsTransformed[8];
public:
	Box() 
	{ 
		m_width = m_height = m_length = m_density = 0.0; 
		Identity(&m_momentOfInertiaTensor); 
		Identity(&m_momentOfInertiaTensorInv);
	}

	Box(double a, double b, double c, double d)
	{
		m_width = a;
		m_height = b;
		m_length = c;
		m_density = d;
		m_massCenter.m_mass = CalculateMass();

		CalculateInertiaTensor();
	}

	double CalculateMass() { m_mass = m_width * m_height * m_length * m_density; return m_mass; }

	void CalculateInertiaTensor()
	{
		CalculateMass();
		Identity(&m_momentOfInertiaTensor);
		m_momentOfInertiaTensor *= (m_mass/12.0);
		m_momentOfInertiaTensor.M00 *= m_height*m_height + m_length*m_length; //Ixx [0]
		m_momentOfInertiaTensor.M11 *= m_width*m_width + m_length*m_length; //Iyy [4] 
		m_momentOfInertiaTensor.M22 *= m_width*m_width + m_height*m_height; //Izz [8]

		Inverse(m_momentOfInertiaTensor, &m_momentOfInertiaTensorInv);
	}

	virtual void UpdateMove()
	{
		RigidBody::UpdateMove();
 
		// front
		m_points[0] = Vec3d( - m_width*0.5,  - m_height*0.5,  + m_length*0.5);
		m_points[1] = Vec3d( - m_width*0.5,  + m_height*0.5,  + m_length*0.5);
		m_points[2] = Vec3d( + m_width*0.5,  + m_height*0.5,  + m_length*0.5);
		m_points[3] = Vec3d( + m_width*0.5,  - m_height*0.5,  + m_length*0.5);

		// back
		m_points[4] = Vec3d( - m_width*0.5,  - m_height*0.5,  - m_length*0.5);
		m_points[5] = Vec3d( - m_width*0.5, + m_height*0.5,  - m_length*0.5);
		m_points[6] = Vec3d( + m_width*0.5,  + m_height*0.5,  - m_length*0.5);
		m_points[7] = Vec3d( + m_width*0.5,  - m_height*0.5,  - m_length*0.5);

		Matrix3d rot;

		#ifdef USE_QUATERNIONS_FOR_ROTATIONS
			m_rotation.ToMatrix3x3(&rot);
		#else
			rot = m_rotation;
		#endif

		for (int i = 0; i < 8; ++i)
		{
			m_pointsTransformed[i] = TransformVec3(rot, m_points[i]);
			m_pointsTransformed[i] += m_massCenter.m_pos;
		}
	}

	void CalculateAABB(AABB *box)
	{
		box->Reset();
		for (int i = 0; i < 8; ++i)
		{
			box->AddPoint(m_pointsTransformed[i]);
		}
	}
};

class RigibBodySet 
{	
protected:
	unsigned int m_count;
	RigidBody *m_bodies;
	/** if true then smulation is not performed for given point, 
	*  by default it is false for every point */
	bool *m_bounds;				
public:
	RigibBodySet(unsigned int count);
	virtual ~RigibBodySet();		

	virtual void ForceAndMomentOfForce(unsigned int id, Vec3d *force, Vec3d *momentOfForce) = 0;
	virtual void BeforeStep(double deltaTime) { };
	virtual void AfterStep(double deltaTime)  { };
	virtual void Update(double deltaTime);

public:
	unsigned int BodiesCount() const { return m_count; }
	RigidBody *Body(unsigned int i) { assert(i < m_count); return &m_bodies[i]; } 
	void SetBound(unsigned int id, bool bound) { assert(id < m_count); m_bounds[id] = bound; }
};