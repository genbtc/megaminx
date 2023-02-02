/** @file camera.h
 *  @brief camera and input functions and getCurrentFaceFromAngles()
 *
 *  @author Bartlomiej Filipek
 *  @date April 2011
 *  //edited by genBTC November 2017
 */
#pragma once

class Camera {
public:
    double m_angleX, m_angleY, m_zoom;
    bool m_isLeftPressed, m_isMiddlePressed;
    int m_mouseX, m_mouseY;
    double m_screenRatio, m_forced_aspect_ratio;
    int m_screenWidth, m_screenHeight;
    //from main.h
    int menuVisibleState = 0;
    bool g_areWeDraggingPoint;
private:
    double m_deltaAngX, m_deltaAngY, m_deltaZoom;
    int m_lastX, m_lastY;
    //from main.h
    unsigned int g_lastHitPointID;
    unsigned int g_draggedPointID;
    int bnstate[16]; //mousebutton
    unsigned int DOUBLE_CLICK_INTERVAL = 400;
    unsigned int prev_left_click;
    int prev_left_x, prev_left_y;

public:
    Camera();
    ~Camera() = default;

    void ChangeViewportSize(int w, int h);
    void PressSpecialKey(int key, int x, int y);

    void ProcessMouse(int button, int state, int x, int y);
    void ProcessMouseMotion(int x, int y, bool calcRotation = true);
    void ProcessPassiveMouseMotion(int x, int y);

    void RotateGLCameraView();
};

int getCurrentFaceFromAngles(int x, int y);

//#include "utils_math.h"
/*
** simple class that can calculate ray into scene from mouse position
** it can be used to perform picking/selection
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
};
*/
