/** @file camera.cpp
 *  @brief common input functions
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 *	@Edited by GENBTC 2017
 */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut_std.h>
#include <algorithm>
#include "utils_math.h"
#include "camera.h"

//////////////////////////////////////////////////////////////////////////
// Camera
//////////////////////////////////////////////////////////////////////////
Camera::Camera() : m_angleX(0), m_angleY(0), m_zoom(0), m_isLeftPressed(false), m_isMiddlePressed(false), m_mouseX(0),
                   m_mouseY(0), m_screenRatio(0), m_forced_aspect_ratio(0), m_screenWidth(0), m_screenHeight(0), 
				   m_deltaAngX(0), m_deltaAngY(0), m_deltaZoom(0), m_lastX(0), m_lastY(0)
{
}

void Camera::ChangeViewportSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;
	
	//Turn on forced aspect ratio of 1.0
	if (m_forced_aspect_ratio == 1)
	{
		const auto minx = std::min((double)w, h*m_forced_aspect_ratio);
		h = w = (int)minx;
	}
	m_screenWidth = w;
	m_screenHeight = h;
	m_screenRatio = m_forced_aspect_ratio * w / h;		

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glutReshapeWindow(w, h);
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);
}

void Camera::PressSpecialKey(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT  : m_angleX += -1.f; break;
		case GLUT_KEY_RIGHT : m_angleX += 1.f; break;
		case GLUT_KEY_UP    : m_angleY += 1.f; break;
		case GLUT_KEY_DOWN  : m_angleY += -1.f; break;
	default: break;
	}
}

void Camera::ProcessMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{		
		if (state == GLUT_DOWN)
		{
			//mouse look controls:
			m_lastX = x;
			m_lastY = y;
			m_deltaAngY = m_angleY;
			m_deltaAngX = m_angleX;
			m_isLeftPressed = true;
		}
		else if (state == GLUT_UP)
			m_isLeftPressed = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN)
			m_isMiddlePressed = true;
		else if (state == GLUT_UP)
			m_isMiddlePressed = false;
	}
}

void Camera::ProcessMouseMotion(int x, int y, bool calcRotation)
{
	m_mouseX = x;
	m_mouseY = y;
	if (calcRotation)
	{
		//Original Implementation:
		m_angleY = m_deltaAngY + (m_lastY - m_mouseY) / 2;
		m_angleX = m_deltaAngX + (m_mouseX - m_lastX) / 2;
	}

}

void Camera::ProcessPassiveMouseMotion(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;
}

/**
 * \brief Required. Call this during/after your Update or render function to enable camera behavior.
 */
void Camera::RotateGLCameraView()
{
	//Prevent over-rotation.
	if (m_angleX >= 360) m_angleX -= 360;
	if (m_angleY >= 360) m_angleY -= 360;
	if (m_angleX < 0) m_angleX += 360;
	if (m_angleY < 0) m_angleY += 360;
    //These must be transformed in this order for mouse to work right.
    glTranslated(0, 0, m_zoom);
	glRotated(m_angleY, -1, 0, 0);
	glRotated(m_angleX, 0, 0, 1);
}

//////////////////////////////////////////////////////////////////////////
// MouseRayTestData
//////////////////////////////////////////////////////////////////////////

MouseRayTestData::MouseRayTestData()
	: m_start(0.0)
	, m_end(0.0)
	, m_lastT(0.0)
	, m_hit(false)
{
}

void MouseRayTestData::CalculateRay(const Camera &cam)
{
	double matModelView[16], matProjection[16];
	int viewport[4];

	glGetDoublev( GL_MODELVIEW_MATRIX, matModelView );
	glGetDoublev( GL_PROJECTION_MATRIX, matProjection );
	glGetIntegerv( GL_VIEWPORT, viewport );

    const double winX = (double)cam.m_mouseX;
    const double winY = viewport[3] - (double)cam.m_mouseY;

	gluUnProject(winX, winY, 0.0, matModelView, matProjection, viewport, &m_start.x, &m_start.y, &m_start.z);
	gluUnProject(winX, winY, 1.0, matModelView, matProjection, viewport, &m_end.x, &m_end.y, &m_end.z);

    const Vec3d v2 = m_end - m_start;
    const Vec3d pt2 = m_start + v2*m_lastT;

	m_point = pt2;

	m_dir = m_end - m_start;
	m_dir.Normalize();
}