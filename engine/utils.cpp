#include <math.h>

static const long double PI = acos(-1);
void rotate_vertex(double &vx, double &vy, double angle)
{
    const double r = sqrt(vx * vx + vy * vy);
	double a = vy > 0 ? acos(vx / r) : 2 * PI - acos(vx / r);
	a += angle;
	vx = r * cos(a);
	vy = r * sin(a);
}

void rotateVertex(double *vertex, char axis, double angle)
{
    //Vector3d v3d = { vertex[0],vertex[1],vertex[2] };
	switch (axis)
	{
	case 'x':
		rotate_vertex(vertex[1], vertex[2], angle);
		break;
	case 'y':
		rotate_vertex(vertex[0], vertex[2], angle);
		break;
	case 'z':
		rotate_vertex(vertex[0], vertex[1], angle);
		break;
    default:
        break;
	}
}