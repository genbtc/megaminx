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
private:
    double m_deltaAngX, m_deltaAngY, m_deltaZoom;
    int m_lastX, m_lastY;

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


/**
 * \brief Static Free function. Takes camera position angles and tells what face
 * is most showing. Shortcut way of angle detection.
 * \param x camera_angleX
 * \param y camera_angleY
 * \return face # color-int (1-12) as result.
 */
static int getCurrentFaceFromAngles(int x, int y)
{
    //Vars:
    constexpr int r = 72;       //face angles
    constexpr int d = r / 2;    //36 half of face
    constexpr int s = 60;  // or match START_ANGLE in main.cpp
    int face = 0;   //color-int (1-12) as result.
    //Angle Conditions:
    const bool y1 = y >= (s - d) && y <= (s + d);                  // 60
    const bool y1b = y >= (s + 240 - d) && y <= (s + 240 + d);      //300 (other opposite)
    const bool y2 = y >= (s + 180 - d) && y <= (s + 180 + d);      //240
    const bool y2b = y >= (s + 60 - d) && y <= (s + 60 + d);      //120 (other opposite)
    const bool y3 = y >= (s + 120 - d) && y <= (s + 120 + d);      //180
    const bool y4a = y >= (0 - d) && y <= (0 + d);                //0
    const bool y4b = y >= (360 - d) && y <= (360 + d);              //360
    //Edited so horizontal mouse-movement isnt backwards anymore when cube is vertically inverted (white face up), reliant on fix w/ Camera.cpp@Line126
    constexpr int toplistA[5] = { 12,11,10,9,8 };
    constexpr int toplistB[5] = { 3,4,5,6,2 };
    constexpr int botlistA[5] = { 11,12,8,9,10 };
    constexpr int botlistB[5] = { 4,3,2,6,5 };
    if (y1 && x < d)
        face = 8;   //LIGHT_BLUE
    else if (y2 && x < d)
        face = 2;   //DARK_BLUE
    if (face) return face;
    for (int i = 0; i < 5; ++i) {
        if (x >= d + r * i && x < d + r * (i + 1)) {
            if (y1)
                face = toplistA[i];
            else if (y2)
                face = toplistB[i];
            if (face) return face;
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (x >= r * i && x < r * (i + 1)) {
            if (y1b)
                face = botlistA[i];
            else if (y2b)
                face = botlistB[i];
            if (face) return face;
        }
    }
    if (y3 && !face)    //Bottom {1}
        face = 1;
    else if ((y4a || y4b) && !face) //Top {7}
        face = 7;
    return face;
}
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
