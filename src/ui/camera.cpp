/** @file camera.cpp
 *  @brief common camera & input functions
 *
 *  @author Bartlomiej Filipek
 *  @date April 2011
 *  @Edited //by genBTC 2017 for megaminx
 */
#include "opengl.h"
#include "camera.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////////
// Camera
//////////////////////////////////////////////////////////////////////////
Camera::Camera() : m_angleX(0), m_angleY(0), m_zoom(0), m_isLeftPressed(false),
    m_isMiddlePressed(false), m_mouseX(0),
    m_mouseY(0), m_screenRatio(0), m_forced_aspect_ratio(0), m_screenWidth(0), m_screenHeight(0),
    m_deltaAngX(0), m_deltaAngY(0), m_deltaZoom(0), m_lastX(0), m_lastY(0)
{
}

void doDoubleClickRotate(int x, int y);

void Camera::ChangeViewportSize(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    //Turn on forced aspect ratio of 1.0
    if (m_forced_aspect_ratio == 1) {
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
    case GLUT_KEY_LEFT  :
        m_angleX += -1.0f;
        break;
    case GLUT_KEY_RIGHT :
        m_angleX += 1.0f;
        break;
    case GLUT_KEY_UP    :
        m_angleY += 1.0f;
        break;
    case GLUT_KEY_DOWN  :
        m_angleY += -1.0f;
        break;
    default:
        break;
    }
}

void Camera::ProcessMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            //mouse look controls:
            m_lastX = x;
            m_lastY = y;
            m_deltaAngY = m_angleY;
            m_deltaAngX = m_angleX;
            m_isLeftPressed = true;
        }
        else if (state == GLUT_UP) {
            m_isLeftPressed = false;
        }
    }
    else if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN)
            m_isMiddlePressed = true;
        else if (state == GLUT_UP)
            m_isMiddlePressed = false;
    }

    // can we move?
    if (m_isLeftPressed) {// && g_rayTest.m_hit) {
        g_draggedPointID = g_lastHitPointID;
        g_areWeDraggingPoint = true;
    }
    else
        g_areWeDraggingPoint = false;

    //Double Click
    bnstate[button] = state == GLUT_DOWN ? 1 : 0;
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            const unsigned int msec = glutGet(GLUT_ELAPSED_TIME);
            const int dx = abs(x - prev_left_x);
            const int dy = abs(y - prev_left_y);

            if (msec - prev_left_click < DOUBLE_CLICK_INTERVAL && dx < 2 && dy < 2) {
                doDoubleClickRotate(x, y);
                prev_left_click = 0;
            }
            else {
                prev_left_click = msec;
                prev_left_x = x;
                prev_left_y = y;
            }
        }
    }
    // Disregard redundant GLUT_UP events
    else if (state == GLUT_UP)
        return;

    //  Mouse Wheels - starts on 0, (usually 4 & 5) now are 3 & 4 
    if (button == 3) {
        //Mouse wheel up
        m_zoom += 5;
    }
    else if (button == 4) {
        //Mouse wheel down
        m_zoom -= 5;
    }
}

extern int menuVisibleState;
void Camera::ProcessMouseMotion(int x, int y, bool calcRotation)
{
    m_mouseX = x;
    m_mouseY = y;
    if (calcRotation) {
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
    //vertical
    glRotated(m_angleY, -1, 0, 0); 
    //horizontal wants to act in reverse when cube is flipped upside down.
    //this fix is crude, works, but causes a visible and disorienting glitch across the transition. 
    if (m_angleY > 180) //cant just go changing this without the getCurrentAngles being modified.
        glRotated(-m_angleX, 0, 0, 1); //rotating the faces CW/CCW opposite when </>180 transition. White Face and gray face pay the consequences
    else //default:
        glRotated(m_angleX, 0, 0, 1);
}



/**
 * \brief Free function. Takes camera position angles and tells what face
 *    is most showing. Shortcut way of angle detection. external linkage.
 * \param x camera_angleX
 * \param y camera_angleY
 * \return face # color-int (1-12) as result.
 */
int getCurrentFaceFromAngles(int x, int y)
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


//////////////////////////////////////////////////////////////////////////
// MouseRayTestData
//////////////////////////////////////////////////////////////////////////
/*
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
*/
