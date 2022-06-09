/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray. 
*/
float Cone::intersect(glm::vec3 pos, glm::vec3 dir)
{
    float a1 = pos.x - center.x;
    float b1 = pos.z - center.z;
    float c1 = height - pos.y + center.y;
    float tanhelp = (radius / height) * (radius / height);
    float a2 = (dir.x * dir.x) + (dir.z * dir.z) - (tanhelp * (dir.y * dir.y));
    float b2 = 2 * ((a1 * dir.x) + (b1 * dir.z) + (tanhelp * c1 * dir.y));
    float c2 = (a1 * a1) + (b1 * b1) - (tanhelp * (c1 * c1));
    float delta = b2 * b2 - 4 * (a2 * c2);
   
	if (fabs(delta) < 0.001) return -1.0;
    if (delta < 0.0) return -1.0;

    float t1 = (-b2 - sqrt(delta)) / (2 * a2);
    float t2 = (-b2 + sqrt(delta)) / (2 * a2);
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
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 d = p - center;
    float r = sqrt((d.x * d.x) + (d.y * d.y));
    glm::vec3 n(d.x, r*(radius/height), d.z);
    n = glm::normalize(n);
    return n;
}
