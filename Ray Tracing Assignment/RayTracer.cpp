/*==================================================================================
* COSC 363  Assignment 2
* Janelle Kwan: jkw81
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Tetrahedron.h"
#include "TextureBMP.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;

vector<SceneObject*> sceneObjects;

TextureBMP texture_sphere, texture_back;

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0.1,0,0);						//Background colour = (0,0,0)
    glm::vec3 lightPos1(15, 35, 0);					//Light's position lightPos
    glm::vec3 lightPos2(-15, 25, 0);
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

    //textured sphere
    if (ray.index == 2) {
        glm::vec3 nv = obj->normal(ray.hit);
        float texs_sphere = nv.x / 2 + 0.5;
        float text_sphere = nv.y / 2 + 0.5;
        color = texture_sphere.getColorAt(texs_sphere, text_sphere);
        obj->setColor(color);
    }

    //cylinder & cone
    if (ray.index == 7 || ray.index == 8) {
        glm::vec3 color1, color2;
        int stripe1 = int(ray.hit.x + ray.hit.y - 5) % 3;
        if (stripe1 == 0) {
            color1 = glm::vec3(0.9, 0.2, 0.6);
        } else color1 = glm::vec3(1, 0.5, 0.8);
        int stripe2 = int(ray.hit.x - ray.hit.y - 5) % 3;
        if (stripe2 == 0) {
            color2 = glm::vec3(0.9, 0.2, 0.6);
        } else color2 = glm::vec3(1, 0.5, 0.8);
        obj->setColor((color1+color2)*0.5f);
    }

    //floor
    if (ray.index == 9)
    {
        //Checker pattern
        int stripeWidth = 5;
        int chkx = int(((ray.hit.x + 30) / stripeWidth)) % 2;
        int chkz = int(((ray.hit.z + 170) / stripeWidth)) % 2;
        if ((chkx && chkz) || (!chkx && !chkz)) {
            color = glm::vec3(1, 1, 1);
        } else {
            color = glm::vec3(0, 0, 0);
        }
        obj->setColor(color);
    }

    //wall
    if (ray.index == 10)
    {
        float texcoords = (ray.hit.x + 30) / (60);
        float texcoordt = (ray.hit.y + 18) / (60);
        color = texture_back.getColorAt(texcoords, texcoordt);
        obj->setColor(color);
    }

    glm::vec3 color1 = obj->lighting(lightPos1, -ray.dir, ray.hit); //Object's colour
    glm::vec3 lightVec1 = lightPos1 - ray.hit;
    Ray shadowRay1(ray.hit, lightVec1);
    shadowRay1.closestPt(sceneObjects);
    float lightDist1 = glm::length(lightVec1);

    glm::vec3 color2 = obj->lighting(lightPos2, -ray.dir, ray.hit);
    glm::vec3 lightVec2 = lightPos2 - ray.hit;
    Ray shadowRay2(ray.hit, lightVec2);
    shadowRay2.closestPt(sceneObjects);
    float lightDist2 = glm::length(lightVec2);

    if (shadowRay1.index == 1 && shadowRay1.dist < lightDist1) {
        color1 = 0.5f * color1;
    }
    if (shadowRay1.index == 3 && shadowRay1.dist < lightDist1) {
        color1 = glm::vec3(0.7f,0.6f,0.6f) * color1;
    }
    if (shadowRay1.index > -1 && shadowRay1.index != 1 && shadowRay1.index != 3 && shadowRay1.dist < lightDist1) {
        color1 = 0.2f * color1; //0.2 = ambient scale factor
    }

    if (shadowRay2.index == 1 && shadowRay2.dist < lightDist2) {
        color2 = 0.5f * color2;
    }
    if (shadowRay2.index == 3 && shadowRay2.dist < lightDist2) {
        color2 = glm::vec3(0.7f,0.6f,0.6f) * color2;
    }
    if (shadowRay2.index > -1 && shadowRay2.index != 1 && shadowRay2.index != 3 && shadowRay2.dist < lightDist2) {
        color2 = 0.2f * color2;
    }
    color = (color1*0.6f)+(color2*0.4f);

    glm::vec3 normalVec = obj->normal(ray.hit);

    if (obj->isReflective() && step < MAX_STEPS)
    {
        float rho = obj->getReflectionCoeff();
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho * reflectedColor);
    }

    if (ray.index == 1 && step < MAX_STEPS) //refractive
    {
        float eta = 1.5;
        glm::vec3 refractedDir1 = glm::refract(ray.dir, normalVec, 1.0f/eta);
        Ray refractedRay1(ray.hit, refractedDir1);
        refractedRay1.closestPt(sceneObjects);
        glm::vec3 m = sceneObjects[refractedRay1.index]->normal(refractedRay1.hit);
        glm::vec3 refractedDir2 = glm::refract(refractedDir1, -m, eta);
        Ray refractedRay2(refractedRay1.hit, refractedDir2);
        glm::vec3 refractedColor = trace(refractedRay2, step + 1);
        color = (1.0f * refractedColor) + (color - 0.8f);
    }

    if (ray.index == 3 && step < MAX_STEPS) //transparent
    {
        Ray refractedRay1(ray.hit, ray.dir);
        refractedRay1.closestPt(sceneObjects);
        Ray refractedRay2(refractedRay1.hit, ray.dir);
        glm::vec3 refractedColor;
        if (ray.dist < 0.1) {
            refractedColor = trace(refractedRay1, step + 1);
        } else refractedColor = trace(refractedRay2, step + 1);
        color = color + (0.8f * refractedColor);
    }
	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
        xp = XMIN + i * cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
            yp = YMIN + j * cellY;

            //no anti-aliasing
            /*glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);
            Ray ray = Ray(eye, dir);
            glm::vec3 col = trace(ray, 1);
            glColor3f(col.r, col.g, col.b);*/

            //anti-aliasing
            glm::vec3 dir1(xp + 0.25 * cellX, yp + 0.25 * cellY, -EDIST);	//direction of the primary ray
            glm::vec3 dir2(xp + 0.25 * cellX, yp + 0.75 * cellY, -EDIST);
            glm::vec3 dir3(xp + 0.75 * cellX, yp + 0.25 * cellY, -EDIST);
            glm::vec3 dir4(xp + 0.75 * cellX, yp + 0.75 * cellY, -EDIST);

		    Ray ray1 = Ray(eye, dir1); Ray ray2 = Ray(eye, dir2);
            Ray ray3 = Ray(eye, dir3); Ray ray4 = Ray(eye, dir4);

		    glm::vec3 col1 = trace(ray1, 1); glm::vec3 col2 = trace(ray2, 1);
            glm::vec3 col3 = trace(ray3, 1); glm::vec3 col4 = trace(ray4, 1);

            float colr = (col1.r + col2.r + col3.r + col4.r) / 4;
            float colg = (col1.g + col2.g + col3.g + col4.g) / 4;
            float colb = (col1.b + col2.b + col3.b + col4.b) / 4;

            glColor3f(colr, colg, colb);

			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
        }
    }

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

    texture_sphere = TextureBMP("Flower.bmp"); //alternatives: Bee, Duck, Flower
    texture_back = TextureBMP("Pumpkin.bmp");

    Sphere *sphere1 = new Sphere(glm::vec3(-10.0, 0.0, -90.0), 10.0); //reflect //index 0
    sphere1->setColor(glm::vec3(0.9, .2, .6));
    sphere1->setReflectivity(true, 0.8);
    sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

    Sphere *sphere2 = new Sphere(glm::vec3(-2.0, -10, -70.0), 4.0); //refract //index 1
    sceneObjects.push_back(sphere2);

    Sphere *sphere3 = new Sphere(glm::vec3(-11.0, -6.5, -65.0), 3); //texture //index 2
    sphere3->setColor(glm::vec3(1, 1, 0.5));
    sceneObjects.push_back(sphere3);

    Sphere *sphere4 = new Sphere(glm::vec3(13.0, 12.0, -110.0), 7.0); //hollow //index 3
    sphere4->setColor(glm::vec3(1, .5, .8));
    sceneObjects.push_back(sphere4);

    Sphere* sphere5 = new Sphere(glm::vec3(-5.0, 14.0, -90.0), 2.0); //extra //index 4
    sphere5->setColor(glm::vec3(0.5, 0, 0.8));
    sceneObjects.push_back(sphere5);

    Sphere* sphere6 = new Sphere(glm::vec3(14.0, -6.0, -75.0), 3.0); //extra //index 5
    sphere6->setColor(glm::vec3(.8, .1, .6));
    sceneObjects.push_back(sphere6);

    Sphere* sphere7 = new Sphere(glm::vec3(21.0, 10.0, -130.0), 3.0); //extra //index 6
    sphere7->setColor(glm::vec3(.8, .1, .8));
    sceneObjects.push_back(sphere7);

    Cylinder* cylinder1 = new Cylinder(glm::vec3(8, -15, -80), 4, 5, glm::vec3(1)); //index 7
    sceneObjects.push_back(cylinder1);

    Cone* cone1 = new Cone(glm::vec3(8, -10, -80), 4, 5, glm::vec3(0, 0, 0)); //index 8
    sceneObjects.push_back(cone1);

    Plane *plane1 = new Plane (glm::vec3(-30., -18, -40), //Point A //index 9
                                glm::vec3(30., -18, -40), //Point B
                                glm::vec3(30., -18, -170), //Point C
                                glm::vec3(-30., -18, -170)); //Point D
    plane1->setColor(glm::vec3(1, 0.3, 0.3));
    sceneObjects.push_back(plane1);
    plane1->setSpecularity(false);

    Plane* plane2 = new Plane(glm::vec3(-30., -18, -170), //Point A //index 10
                                glm::vec3(30., -18, -170), //Point B
                                glm::vec3(30., 42, -170), //Point C
                                glm::vec3(-30., 42, -170)); //Point D
    plane2->setColor(glm::vec3(1, 1, 1));
    sceneObjects.push_back(plane2);
    plane2->setSpecularity(false);

    glm::vec3 tet_a(8 ,-1, -76);
    glm::vec3 tet_b(11, -1, -83);
    glm::vec3 tet_c(5, -1, -83);
    glm::vec3 tet_top(8, 3, -80);
    glm::vec3 tet_bot(8, -5, -80);
    Tetrahedron* tetrahedron1 = new Tetrahedron(tet_a, tet_top, tet_c); //index 11
    tetrahedron1->setColor(glm::vec3(0, 1, 1));
    sceneObjects.push_back(tetrahedron1);
    Tetrahedron* tetrahedron2 = new Tetrahedron(tet_c, tet_top, tet_b); //index 12
    tetrahedron2->setColor(glm::vec3(0, 1, 1));
    sceneObjects.push_back(tetrahedron2);
    Tetrahedron* tetrahedron3 = new Tetrahedron(tet_b, tet_top, tet_a); //index 13
    tetrahedron3->setColor(glm::vec3(0, 1, 1));
    sceneObjects.push_back(tetrahedron3);
    Tetrahedron* tetrahedron4 = new Tetrahedron(tet_a, tet_c, tet_bot); //index 14
    tetrahedron4->setColor(glm::vec3(0, 1, 1));
    sceneObjects.push_back(tetrahedron4);
    Tetrahedron* tetrahedron5 = new Tetrahedron(tet_c, tet_b, tet_bot); //index 15
    tetrahedron5->setColor(glm::vec3(0, 1, 1));
    sceneObjects.push_back(tetrahedron5);
    Tetrahedron* tetrahedron6 = new Tetrahedron(tet_b, tet_a, tet_bot); //index 16
    tetrahedron6->setColor(glm::vec3(0, 1, 1));
    sceneObjects.push_back(tetrahedron6);
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Assignment 2");

    glutDisplayFunc(display);
    initialize();
    glutMainLoop();
    return 0;
}
