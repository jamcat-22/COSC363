/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray. 
*/
float Cylinder::intersect(glm::vec3 pos, glm::vec3 dir)
{
    glm::vec3 jk = pos - center;
    float a = (dir.x * dir.x) + (dir.z * dir.z);
    float b = 2 * ((jk.x * dir.x) + (jk.z * dir.z));
    float c = (jk.x * jk.x) + (jk.z * jk.z) - (radius * radius);
    float delta = (b * b) - 4 * (a * c);
   
	if (fabs(delta) < 0.001) return -1.0;
    if (delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    float t_1, t_2;

    if (t1 > t2) {
        t_1 = t2;
        t_2 = t1;
    } else {
        t_1 = t1;
        t_2 = t2;
    }

    float rd1 = pos.y + (dir.y * t_1);
    float rd2 = pos.y + (dir.y * t_2);

    if ((rd1 > center.y) and (rd1 < center.y + height)) {
        return t_1;
    } else if ((rd2 > center.y) and (rd2 < center.y + height)) {
        return t_2;
    } else return -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 d = p - center;
    glm::vec3 n(d.x, 0, d.z);
    n = glm::normalize(n);
    return n;
}
