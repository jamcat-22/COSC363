/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The tetrahedron class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_TETRAHEDRON
#define H_TETRAHEDRON
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a tetrahedron
 */
class Tetrahedron : public SceneObject
{
private:
    glm::vec3 one;
    glm::vec3 two;
    glm::vec3 three;

public:
    Tetrahedron(void);

    Tetrahedron(glm::vec3 a, glm::vec3 b, glm::vec3 c) : one(a), two(b), three(c) {
        glm::vec3 color(1);
    }
    bool isInside(glm::vec3 pos);
	float intersect(glm::vec3 pos, glm::vec3 dir);
	glm::vec3 normal(glm::vec3 p);
};

#endif //!H_TETRAHEDRON
