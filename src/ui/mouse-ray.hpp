#include "utils_math.h"
#include "camera.hpp"
/*
** simple class that can calculate ray into scene from mouse position
** it can be used to perform picking/selection, see mouse-ray.cpp
*/
class MouseRayTestData {
public:
    MouseRayTestData();

    // start pos of the ray - usually located in the near plane
    Vec3d m_start;
    // end pos of the ray - usually located in the far plane
    Vec3d m_end;
    // normalized direction from start to end
    Vec3d m_dir;
    // mouse point in 3D scene, based on lastT
    Vec3d m_point;
    // scalar that means position on the ray where mouse hit something
    double m_lastT;
    // did we hit something ?
    bool m_hit;

    // calculates ray for the mouse, that ray can be used to perform picking
    // when hit is found the remember to set m_lastT param
    void CalculateRay(const Camera &cam);
    //
    Vec3d get_normal(Vec3d x0, Vec3d x1, Vec3d x2);
};