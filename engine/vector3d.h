#pragma once
#include <math.h>
class Vector3f
{
public:
    float x, y, z;

    // Constructors as well as getters/setters omitted for brevity!!
    // Only important methods kept necessary for this tutorial.
    // The original class contains many more methods...
    float vec[3];

    float* get()
    {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        return vec;
    }
    Vector3f add(Vector3f a) {
        x += a.x;
        y += a.y;
        z += a.z;
        return { x,y,z };
    }

    Vector3f set(Vector3f v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return { x,y,z };
    }

    Vector3f subAndAssign(Vector3f a, Vector3f b) {
        x = a.x - b.x;
        y = a.y - b.y;
        z = a.z - b.z;
        return {x,y,z};
    }

    /**
    * Returns the length of the vector, also called L2-Norm or Euclidean Norm.
    */
    float l2Norm() {
        return (float)sqrt(x*x + y*y + z*z);
    }

    Vector3f crossAndAssign(Vector3f a, Vector3f b) {
        float tempX = a.y * b.z - a.z * b.y;
        float tempY = a.z * b.x - a.x * b.z;
        float tempZ = a.x * b.y - a.y * b.x;

        x = tempX;
        y = tempY;
        z = tempZ;
        return { x,y,z };
    }

    Vector3f scale(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return { x,y,z };
    }

    Vector3f normalize() {
        float length = l2Norm();
        x /= length;
        y /= length;
        z /= length;
        return { x,y,z };
    }
};
class Vector3d
{
public:
    double x, y, z;
    double vec[3];

    double* get()
    {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        return vec;
    }

    Vector3d* add(Vector3d a) {
        x += a.x;
        y += a.y;
        z += a.z;

        return this;
    }

    Vector3d* set(Vector3d v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;

        return this;
    }

    Vector3d* subAndAssign(Vector3d a, Vector3d b) {
        x = a.x - b.x;
        y = a.y - b.y;
        z = a.z - b.z;

        return this;
    }

    /**
    * Returns the length of the vector, also called L2-Norm or Euclidean Norm.
    */
    double l2Norm() {
        return (double)sqrt(x*x + y*y + z*z);
    }

    Vector3d* crossAndAssign(Vector3d a, Vector3d b) {
        const double tempX = a.y * b.z - a.z * b.y;
        const double tempY = a.z * b.x - a.x * b.z;
        const double tempZ = a.x * b.y - a.y * b.x;

        x = tempX;
        y = tempY;
        z = tempZ;

        return this;
    }

    Vector3d* scale(double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;

        return this;
    }

    Vector3d* normalize() {
        const double length = l2Norm();
        x /= length;
        y /= length;
        z /= length;

        return this;
    }
};
class PickingRay
{
private:    //setters
    Vector3f clickPosInWorld;
    Vector3f direction;
public:     //getters
    Vector3f getClickPosInWorld() {
        return clickPosInWorld;
    }
    Vector3f getDirection() {
        return direction;
    }

    /**
    * Computes the intersection of this ray with the X-Y Plane (where Z = 0)
    * and writes it back to the provided vector.
    */
    void intersectionWithXyPlane(float* worldPos)
    {
        float s = -clickPosInWorld.z / direction.z;
        worldPos[0] = clickPosInWorld.x + direction.x*s;
        worldPos[1] = clickPosInWorld.y + direction.y*s;
        worldPos[2] = 0;
    }


};
class PickingRay3d
{
private:
    Vector3d* clickPosInWorld = new Vector3d();
    Vector3d* direction = new Vector3d();
public:
    /**
    * Computes the intersection of this ray with the X-Y Plane (where Z = 0)
    * and writes it back to the provided vector->
    */
    void intersectionWithXyPlane(double* worldPos)
    {
        float s = -clickPosInWorld->z / direction->z;
        worldPos[0] = clickPosInWorld->x + direction->x*s;
        worldPos[1] = clickPosInWorld->y + direction->y*s;
        worldPos[2] = 0;
    }

    Vector3d* getClickPosInWorld() {
        return clickPosInWorld;
    }
    Vector3d* getDirection() {
        return direction;
    }
};