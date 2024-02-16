/** @file camera.h
 *  @brief camera and input functions and getCurrentFaceFromAngles()
 *  @author originally Bartlomiej Filipek
 *  @date April 2011
 *  @author edited by genBTC November 2017-2023
 */
#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera {
private:
    double m_deltaAngX, m_deltaAngY, m_deltaZoom;
    int m_lastX, m_lastY;
    //from main.h
    unsigned int m_lastHitPointID, m_draggedPointID;
    unsigned int DOUBLE_CLICK_INTERVAL = 400;
    unsigned int prev_left_click;
    int prev_left_x, prev_left_y;

public:
    double m_angleX, m_angleY, m_zoom;
    bool m_isLeftPressed, m_isMiddlePressed;
    int m_mouseX, m_mouseY;
    double m_screenRatio, m_forced_aspect_ratio;
    int m_screenWidth, m_screenHeight;
    //from main.h
    int menuVisibleState = 0;
    bool m_areWeDraggingPoint;
    bool isSpinning = false;

    Camera();
    ~Camera() = default;

    void ChangeViewportSize(int w, int h);
    void PressSpecialKey(int key, int x, int y);

    void ProcessMouse(int button, int state, int x, int y);
    void ProcessMouseMotion(int x, int y, bool calcRotation = true);
    void ProcessPassiveMouseMotion(int x, int y);

    void RotateGLCameraView();
};

//main.cpp prototype
int getCurrentFaceFromAngles(int x, int y);

#endif //__CAMERA_H__
