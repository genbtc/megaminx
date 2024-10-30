//////////////////////////////////////////////////////////////////////////
// MouseRayTestData
//////////////////////////////////////////////////////////////////////////
#include "mouse-ray.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

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

    gluUnProject(winX, winY, 0.0, matModelView, matProjection, viewport, &m_start.x, &m_start.y,
                 &m_start.z);
    gluUnProject(winX, winY, 1.0, matModelView, matProjection, viewport, &m_end.x, &m_end.y, &m_end.z);

    const Vec3d v2 = m_end - m_start;
    const Vec3d pt2 = m_start + v2*m_lastT;

    m_point = pt2;

    m_dir = m_end - m_start;
    m_dir.Normalize();
}

Vec3d MouseRayTestData::get_normal(Vec3d x0, Vec3d x1, Vec3d x2) {
  Vec3d v0 = x0 - x2;
  Vec3d v1 = x1 - x2;
  Vec3d n = Vec3d::CrossProduct(v0,v1);
  n.Normalize();
  return n;
}