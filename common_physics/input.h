/** @file input.h
 *  @brief common input functions
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#pragma once
#include "utils_math.h"

class Camera
{
public:
	float m_angleX, m_angleY, m_zoom;
	float m_deltaAngX, m_deltaAngY, m_deltaZoom;

	bool m_isLeftPressed;
	bool m_isMiddlePressed;
	int	m_lastX;
	int m_lastY;
	int	m_mouseX;
	int m_mouseY;

	int m_screenWidth;
	int m_screenHeight;
	float m_screenRatio;
	float m_forced_aspect_ratio;
public:
	Camera();
	~Camera() { }

	void ChangeViewportSize(int w, int h);
	void PressSpecialKey(int key, int x, int y);
	void ReleaseSpecialKey(int key, int x, int y);
	void ProcessMouse(int button, int state, int x, int y);
	void ProcessMouseMotion(int x, int y, bool calcRotation = true);
	void ProcessPassiveMouseMotion(int x, int y);

	void Update(double deltaTime);
	void SetSimpleView() const;
};

/** simple class that can calculate ray into scene from mouse position
 * it can be used to perform picking/selection */
class MouseRayTestData
{
public:
	/** start pos of the ray - usually located in the near plane */
	Vec3d m_start;
	/** end pos of the ray - usually located in the far plane */
	Vec3d m_end;
	/** normalized direction from start to end */
	Vec3d m_dir;
	/** mouse point in 3D scene, based on lastT */
	Vec3d m_point;
	/** scalar that means position on the ray where mouse hit something */
	double m_lastT;
	/** dit we hit something */
	bool m_hit;
public:
	MouseRayTestData();

	/** calculates ray for the mouse, that ray can be used to perform picking
	  * when hit is found the remember to set m_lastT param */
	void CalculateRay(const Camera &cam);
};



		