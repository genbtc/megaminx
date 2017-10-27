#pragma once
#include <math.h>
class Vector3d
{
public:
    double x, y, z;

    // Constructors as well as getters/setters omitted for brevity!!
    // Only important methods kept necessary for this tutorial.
    // The original class contains many more methods...

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