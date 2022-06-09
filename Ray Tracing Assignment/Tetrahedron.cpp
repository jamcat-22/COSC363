/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The tetrahedron class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Tetrahedron.h"
#include <math.h>

/**
* Tetrahedron's intersection method.  The input is a ray (pos, dir).
*/
float Tetrahedron::intersect(glm::vec3 pos, glm::vec3 dir)
{
	glm::vec3 n = normal(pos);
    glm::vec3 vdif = one - pos;
	float d_dot_n = glm::dot(dir, n);
	if(fabs(d_dot_n) < 1.e-4) return -1;   //Parallel ray
    float t = glm::dot(vdif, n)/d_dot_n;
	if(fabs(t) < 0.0001) return -1;
    glm::vec3 q = pos + dir*t; //Point of intersection
    if (isInside(q)) return t; //Inside the plane
    else return -1; //Outside
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the plane.
*/
glm::vec3 Tetrahedron::normal(glm::vec3 p)
{
    glm::vec3 v1 = two - one;
    glm::vec3 v2 = three - one;
	glm::vec3 n = glm::cross(v1, v2);
	n = glm::normalize(n);
    return n;
}

/**
* 
* Checks if a point q is inside the current polygon
*/
bool Tetrahedron::isInside(glm::vec3 q)
{
	glm::vec3 n = normal(q);     //Normal vector at the point of intersection
    glm::vec3 ua = two - one, ub = three - two, uc = one - three;
    glm::vec3 va = q - one, vb = q - two, vc = q - three;
    float ka = glm::dot(glm::cross(ua, va), n);
	float kb = glm::dot(glm::cross(ub, vb), n);
	float kc = glm::dot(glm::cross(uc, vc), n);
	if (ka > 0 && kb > 0 && kc > 0) return true;
	else return false;
}



