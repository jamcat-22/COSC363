#include <iostream>
#include <fstream>
#include <climits>
#include <cmath>
#include <GL/freeglut.h>
#include "RailModels.h"
#include "loadBMP.h"
using namespace std;

GLuint txId[5];

//load textures
void loadTexture()
{
    glGenTextures(5, txId);   //Get 5 texture IDs
    glBindTexture(GL_TEXTURE_2D, txId[0]);  //Concrete texture
    loadBMP("concrete.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[1]);  //Roof texture
    loadBMP("roof.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[2]);  //Brick texture
    loadBMP("brick.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[3]);  //Wood texture
    loadBMP("wood.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[4]);  //Train texture
    loadBMP("traintex.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//normal 3 points
void normal(float x1, float y1, float z1,
            float x2, float y2, float z2,
              float x3, float y3, float z3 )
{
      float nx, ny, nz;
      nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
      ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
      nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);
      glNormal3f(nx, ny, nz);
}

//normal for 2d
void normal_2d(std::pair<float, float>& vector)
{
      float magn = sqrt(vector.first*vector.first + vector.second*vector.second);
      vector.first = vector.first/magn;
      vector.second = vector.second/magn;
}

//floor 400x800grid
void floor()
{
    glColor4f(0.0, 0.5, 0.0, 1.0);  //The floor is green
	glNormal3f(0.0, 1.0, 0.0);

	glBegin(GL_QUADS);
    for(int i = -200; i < 200; i++) {
        for(int j = -400;  j < 400; j++) {
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j+1);
			glVertex3f(i+1, 0, j+1);
			glVertex3f(i+1, 0, j);
		}
    }
    glEnd();

}

//track generation
void tracks(int npoints, float *x, float *z)
{
    float w1 = 5; //inner width
    float w2 = 6; //outer width
    float ax1,az1, ax2,az2, ax3,az3, ax4,az4;
    float bx1,bz1, bx2,bz2, bx3,bz3, bx4,bz4;

    glColor4f(0.3, 0.3, 0.3, 1.0); //grey tracks
    glBegin(GL_QUADS);
    for (int i = 0; i < npoints; i++)
    {
        std::pair<float, float> p0 = {x[i%npoints], z[i%npoints]};
        std::pair<float, float> p1 = {x[(i+1)%npoints], z[(i+1)%npoints]};
        std::pair<float, float> p2 = {x[(i+2)%npoints], z[(i+2)%npoints]};
		
        std::pair<float, float> u0 = {(p1.first - p0.first), (p1.second - p0.second)};
		normal_2d(u0);
        std::pair<float, float> v0 = {u0.second, -u0.first};
		
        std::pair<float, float> u1 = {(p2.first - p1.first), (p2.second - p1.second)};
		normal_2d(u1);
        std::pair<float, float> v1 = {u1.second, -u1.first};

        ax1 = p0.first + v0.first*w1; az1 = p0.second + v0.second*w1;
        ax2 = p0.first + v0.first*w2; az2 = p0.second + v0.second*w2;
        ax3 = p1.first + v1.first*w1; az3 = p1.second + v1.second*w1;
        ax4 = p1.first + v1.first*w2; az4 = p1.second + v1.second*w2;
		
        bx1 = p0.first - v0.first*w1; bz1 = p0.second - v0.second*w1;
        bx2 = p0.first - v0.first*w2; bz2 = p0.second - v0.second*w2;
        bx3 = p1.first - v1.first*w1; bz3 = p1.second - v1.second*w1;
        bx4 = p1.first - v1.first*w2; bz4 = p1.second - v1.second*w2;
		
		//a quad
        glNormal3f(0., 1., 0.); //top
		glVertex3f(ax1, 1.0, az1);
		glVertex3f(ax2, 1.0, az2);
		glVertex3f(ax4, 1.0, az4);
		glVertex3f(ax3, 1.0, az3);

        glNormal3f(-v0.first, 0., -v0.second);
		glVertex3f(ax1, 0.0, az1);
		glVertex3f(ax1, 1.0, az1);
        glNormal3f(-v1.first, 0., -v1.second);
		glVertex3f(ax3, 1.0, az3);
		glVertex3f(ax3, 0.0, az3);

        glNormal3f(v0.first, 0., v0.second);
		glVertex3f(ax2, 1.0, az2);
		glVertex3f(ax2, 0.0, az2);
        glNormal3f(v1.first, 0., v1.second);
        glVertex3f(ax4, 0.0, az4);
        glVertex3f(ax4, 1.0, az4);
		
		//b quad
        glNormal3f(0., 1., 0.); //top
		glVertex3f(bx1, 1.0, bz1);
        glVertex3f(bx3, 1.0, bz3);
		glVertex3f(bx4, 1.0, bz4);
        glVertex3f(bx2, 1.0, bz2);

        glNormal3f(-v0.first, 0., -v0.second);
		glVertex3f(bx2, 1.0, bz2);
        glVertex3f(bx4, 1.0, bz4);
        glNormal3f(-v1.first, 0., -v1.second);
		glVertex3f(bx4, 0.0, bz4);
		glVertex3f(bx2, 0.0, bz2);

        glNormal3f(v0.first, 0., v0.second);
		glVertex3f(bx1, 1.0, bz1);
		glVertex3f(bx1, 0.0, bz1);
        glNormal3f(v1.first, 0., v1.second);
		glVertex3f(bx3, 0.0, bz3);
		glVertex3f(bx3, 1.0, bz3);

	}
	glEnd();
}

//sleeper generation
void sleepers(int npoints, float *x, float *z)
{
    glColor4f(0.5, 0.3, 0, 1); //brown
    glBegin(GL_QUADS);
    int length = 7;
    int width = 2;
    float sx1, sx2, sx3, sx4, sz1, sz2, sz3, sz4;
    for (int i = 0; i < npoints; i+=2)
    {
        std::pair<float, float> p0 = {x[i%npoints], z[i%npoints]};
        std::pair<float, float> p1 = {x[(i+2)%npoints], z[(i+2)%npoints]};

        std::pair<float, float> u0 = {(p1.first - p0.first), (p1.second - p0.second)};
        normal_2d(u0);
        std::pair<float, float> v0 = {u0.second, -u0.first};

        sx1 = p0.first + v0.first*length; sz1 = p0.second + v0.second*length;
        sx2 = p0.first - v0.first*length; sz2 = p0.second - v0.second*length;
        sx3 = (p0.first + u0.first*width) + v0.first*length; sz3 = (p0.second + u0.second*width) + v0.second*length;
        sx4 = (p0.first + u0.first*width) - v0.first*length; sz4 = (p0.second + u0.second*width) - v0.second*length;

        glNormal3f(0., 1., 0.);
        glVertex3f(sx1, .1, sz1);
        glVertex3f(sx3, .1, sz3);
        glVertex3f(sx4, .1, sz4);
        glVertex3f(sx2, .1, sz2);
    }
    glEnd();

}

//base for wagon and engine 30 length x 1 width
void base()
{
    glColor4f(0.5, 0.5, 0.5, 1.0); //lighter grey
    glPushMatrix();
      glTranslatef(0.0, 4.0, 0.0);
      glScalef(30.0, 2.0, 11.0); //Size 30x10 units, thickness 2 units.
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();	//Connector between wagons
      glTranslatef(16.0, 4.0, 0.0);
      glutSolidCube(2.0);
    glPopMatrix();

    //4 Wheels (radius = 2 units)
	//x, z positions of wheels:
    float wx[4] = {-12, 12, -12, 12};
    float wz[4] = {5.4, 5.4, -5.4, -5.4};
    GLUquadric *q = gluNewQuadric(); //Disc
    glColor4f(0.1, 0.1, 0.1, 1.0); //black wheels
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(wx[i], 2.0, wz[i]);
		gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}
}

//custom engine with textures
void engine()
{
    base();

    //top
    glColor4f(0.1, 0.1, 0.1, 1.0);
    glBegin(GL_QUADS);
        normal(14, 14, 5.5, -5, 14, 5.5, -5, 16, 3.5);
        glVertex3f(14, 14, 5.5);
        glVertex3f(-5, 14, 5.5);
        glVertex3f(-5, 16, 3.5);
        glVertex3f(14, 16, 3.5);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(14, 16, 3.5);
        glVertex3f(-5, 16, 3.5);
        glVertex3f(-5, 16, -3.5);
        glVertex3f(14, 16, -3.5);
    glEnd();

    glBegin(GL_QUADS);
        normal(-5, 14, -5.5, 14, 14, -5.5, 14, 16, -3.5);
        glVertex3f(-5, 14, -5.5);
        glVertex3f(14, 14, -5.5);
        glVertex3f(14, 16, -3.5);
        glVertex3f(-5, 16, -3.5);
    glEnd();

    //corners
    glBegin(GL_TRIANGLES);
        normal(-15, 7, -5.5, -5, 16, -3.5, -5, 14, -5.5);
        glVertex3f(-15, 7, -5.5);
        glVertex3f(-5, 16, -3.5);
        glVertex3f(-5, 14, -5.5);
    glEnd();

    glBegin(GL_TRIANGLES);
        normal(-15, 7, 5.5, -5, 16, 3.5, -5, 14, 5.5);
        glVertex3f(-15, 7, 5.5);
        glVertex3f(-5, 16, 3.5);
        glVertex3f(-5, 14, 5.5);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[4]);

    //back sides
    glColor4f(1, 1, 1, 1.0);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0.324, 0.3125); glVertex3f(-5, 5, 5.5);
        glTexCoord2f(1, 0.3125); glVertex3f(14, 5, 5.5);
        glTexCoord2f(1, 0.6); glVertex3f(14, 14, 5.5);
        glTexCoord2f(0.324, 0.6); glVertex3f(-5, 14, 5.5);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0.324, 0.3125); glVertex3f(14, 5, -5.5);
        glTexCoord2f(1, 0.3125); glVertex3f(-5, 5, -5.5);
        glTexCoord2f(1, 0.6); glVertex3f(-5, 14, -5.5);
        glTexCoord2f(0.324, 0.6); glVertex3f(14, 14, -5.5);
    glEnd();

    //front sides
    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0.324, 0.3125); glVertex3f(-5, 5, -5.5);
        glTexCoord2f(0, 0.3125); glVertex3f(-15, 5, -5.5);
        glTexCoord2f(0, 0.383); glVertex3f(-15, 7, -5.5);
        glTexCoord2f(0.324, 0.6); glVertex3f(-5, 14, -5.5);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0.3125); glVertex3f(-15, 5, 5.5);
        glTexCoord2f(0.324, 0.3125); glVertex3f(-5, 5, 5.5);
        glTexCoord2f(0.324, 0.6); glVertex3f(-5, 14, 5.5);
        glTexCoord2f(0, 0.383); glVertex3f(-15, 7, 5.5);
    glEnd();

    //front nose
    glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(1, 0.61); glVertex3f(-15, 5, -5.5);
        glTexCoord2f(1, 1); glVertex3f(-15, 5, 5.5);
        glTexCoord2f(0.926, 1); glVertex3f(-15, 7, 5.5);
        glTexCoord2f(0.926, 0.61); glVertex3f(-15, 7, -5.5);
    glEnd();

    //windscreen
    glBegin(GL_QUADS);
        normal(-15, 7, -5.5, -15, 7, 5.5, -5, 16, 3.5);
        glTexCoord2f(0.4, 0.687); glVertex3f(-15, 7, -5.5);
        glTexCoord2f(0.78, 0.687); glVertex3f(-15, 7, 5.5);
        glTexCoord2f(0.78, 1); glVertex3f(-5, 16, 3.5);
        glTexCoord2f(0.4, 1); glVertex3f(-5, 16, -3.5);
    glEnd();

    //back
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0.645); glVertex3f(14, 5, 5.5);
        glTexCoord2f(0.387, 0.645); glVertex3f(14, 5, -5.5);
        glTexCoord2f(0.387, 0.93); glVertex3f(14, 14, -5.5);
        glTexCoord2f(0.316, 1); glVertex3f(14, 16, -3.5);
        glTexCoord2f(0.07, 1); glVertex3f(14, 16, 3.5);
        glTexCoord2f(0, 0.93); glVertex3f(14, 14, 5.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//custom wagon with textures
void wagon()
{
    base();

    //top
    glColor4f(0.1, 0.1, 0.1, 1.0);
    glBegin(GL_QUADS);
        normal(14, 14, 5.5, -14, 14, 5.5, -14, 16, 3.5);
        glVertex3f(14, 14, 5.5);
        glVertex3f(-14, 14, 5.5);
        glVertex3f(-14, 16, 3.5);
        glVertex3f(14, 16, 3.5);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(14, 16, 3.5);
        glVertex3f(-14, 16, 3.5);
        glVertex3f(-14, 16, -3.5);
        glVertex3f(14, 16, -3.5);
    glEnd();

    glBegin(GL_QUADS);
        normal(14, 14, -5.5, -14, 14, -5.5, -14, 16, -3.5);
        glVertex3f(-14, 14, -5.5);
        glVertex3f(14, 14, -5.5);
        glVertex3f(14, 16, -3.5);
        glVertex3f(-14, 16, -3.5);
    glEnd();

    //sides
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    glColor4f(1, 1, 1, 1.0);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-14, 5, 5.5);
        glTexCoord2f(1, 0); glVertex3f(14, 5, 5.5);
        glTexCoord2f(1, 0.286); glVertex3f(14, 14, 5.5);
        glTexCoord2f(0, 0.286); glVertex3f(-14, 14, 5.5);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0); glVertex3f(14, 5, -5.5);
        glTexCoord2f(1, 0); glVertex3f(-14, 5, -5.5);
        glTexCoord2f(1, 0.286); glVertex3f(-14, 14, -5.5);
        glTexCoord2f(0, 0.286); glVertex3f(14, 14, -5.5);
    glEnd();

    //back and front
    glColor4f(0.5, 0.5, 0.5, 1.0);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0.645); glVertex3f(14, 5, 5.5);
        glTexCoord2f(0.387, 0.645); glVertex3f(14, 5, -5.5);
        glTexCoord2f(0.387, 0.93); glVertex3f(14, 14, -5.5);
        glTexCoord2f(0.316, 1); glVertex3f(14, 16, -3.5);
        glTexCoord2f(0.07, 1); glVertex3f(14, 16, 3.5);
        glTexCoord2f(0, 0.93); glVertex3f(14, 14, 5.5);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0.645); glVertex3f(-14, 5, -5.5);
        glTexCoord2f(0.387, 0.645); glVertex3f(-14, 5, 5.5);
        glTexCoord2f(0.387, 0.93); glVertex3f(-14, 14, 5.5);
        glTexCoord2f(0.316, 1); glVertex3f(-14, 16, 3.5);
        glTexCoord2f(0.07, 1); glVertex3f(-14, 16, -3.5);
        glTexCoord2f(0, 0.93); glVertex3f(-14, 14, -5.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//curved tunnel piece with textures
void tunnel_piece(float *vx, float *vy, float *vz, int npoints, bool in)//7 degrees
{
    int nslices = 40;
    float angle = .0393; //angle of rotation in radians //90/40slices
    float wx[npoints] = {};
    float wy[npoints] = {};
    float wz[npoints] = {};
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    for (int slice = 0; slice < nslices; slice++) {
        for (int i = 0; i < npoints; i++) {
            wx[i] = vx[i] * cos(angle) + (vz[i]+5.89) * sin(angle); //5.89 from circumference/4(for 90 angle)/slices
            wy[i] = vy[i];
            wz[i] = (vz[i]+5.89) * cos(angle) - vx[i] * sin(angle);
        }
        glBegin(GL_QUAD_STRIP);
            for (int i = 1; i < npoints; i++) {
                if (in == false) normal(vx[i], vy[i], vz[i], vx[i-1], vy[i-1], vz[i-1], wx[i], wy[i], wz[i]);
                if (in == true) normal(vx[i], vy[i], vz[i], wx[i], wy[i], wz[i], vx[i-1], vy[i-1], vz[i-1]);
                if (i == 1) { //first wall
                    glColor4f(.3, .3, .3, 1.0);
                    glTexCoord2f(0, 0); glVertex3f(vx[0], vy[0], vz[0]);
                    glTexCoord2f(1, 0); glVertex3f(wx[0], wy[0], wz[0]);
                    glTexCoord2f(0, 1); glVertex3f(vx[1], vy[1], vz[1]);
                    glTexCoord2f(1, 1); glVertex3f(wx[1], wy[1], wz[1]);
                    glColor4f(.6, .6, .6, 1.0);
                } else if (i > npoints-3) { //last wall
                    glTexCoord2f(0, i-npoints-2); glVertex3f(vx[i], vy[i], vz[i]);
                    glTexCoord2f(1, i-npoints-2); glVertex3f(wx[i], wy[i], wz[i]);
                } else if (i == 2) { //join between first wall and next top
                    glTexCoord2f(0, 0); glVertex3f(vx[1], vy[1], vz[1]);
                    glTexCoord2f(1, 0); glVertex3f(wx[1], wy[1], wz[1]);
                    glTexCoord2f(0, (float)(i-1)/(npoints-1)); glVertex3f(vx[i], vy[i], vz[i]);
                    glTexCoord2f(1, (float)(i-1)/(npoints-1)); glVertex3f(wx[i], wy[i], wz[i]);
                } else if (i == npoints-3) { //join between last top and wall
                    glTexCoord2f(0, (float)(i-1)/(npoints-1)); glVertex3f(vx[i], vy[i], vz[i]);
                    glTexCoord2f(1, (float)(i-1)/(npoints-1)); glVertex3f(wx[i], wy[i], wz[i]);
                    glTexCoord2f(0, (float)(i)/(npoints-1)); glVertex3f(vx[i+1], vy[i+1], vz[i+1]);
                    glTexCoord2f(1, (float)(i)/(npoints-1)); glVertex3f(wx[i+1], wy[i+1], wz[i+1]);
                } else { //top
                    glTexCoord2f(0, (float)(i-1)/(npoints-1)); glVertex3f(vx[i], vy[i], vz[i]);
                    glTexCoord2f(1, (float)(i-1)/(npoints-1)); glVertex3f(wx[i], wy[i], wz[i]);
                }

            }
        glEnd();
        for (int i = 0; i < npoints; i++) {
            vx[i] = wx[i];
            vy[i] = wy[i];
            vz[i] = wz[i];
        }
    }
    glDisable(GL_TEXTURE_2D);
}

//complete tunnel with textures
void tunnel()
{
    int inpoints = 15;
    int outpoints = 17;
    float stinx[15] = {-8, -8, -6.883, -5.634, -4.3, -2.903, -1.462, 0, 1.462, 2.903, 4.3, 5.634, 6.883, 8, 8};
    float stiny[15] = {0, 16, 16.886, 17.651, 18.259, 18.7, 18.967, 19.056, 18.967, 18.7, 18.259, 17.651, 16.886, 16, 0};
    float stoutx[17] = {-10, -10, -8.699, -7.456, -6.103, -4.659, -3.145, -1.584, 0, 1.584, 3.145, 4.659, 6.103, 7.456, 8.699, 10, 10};
    float stouty[17] = {0, 17, 18.354, 19.342, 20.172, 20.83, 21.307, 21.596, 21.693, 21.596, 21.307, 20.83, 20.172, 19.342, 18.354, 17, 0};
    float inx[15] = {-8, -8, -6.883, -5.634, -4.3, -2.903, -1.462, 0, 1.462, 2.903, 4.3, 5.634, 6.883, 8, 8};
    float iny[15] = {0, 16, 16.886, 17.651, 18.259, 18.7, 18.967, 19.056, 18.967, 18.7, 18.259, 17.651, 16.886, 16, 0};
    float inz[15] = {0};
    float outx[17] = {-10, -10, -8.699, -7.456, -6.103, -4.659, -3.145, -1.584, 0, 1.584, 3.145, 4.659, 6.103, 7.456, 8.699, 10, 10};
    float outy[17] = {0, 17, 18.354, 19.342, 20.172, 20.83, 21.307, 21.596, 21.693, 21.596, 21.307, 20.83, 20.172, 19.342, 18.354, 17, 0};
    float outz[17] = {0};
    tunnel_piece(outx, outy, outz, outpoints, false); //outside tunnel
    glColor4f(.5, .5, .5, 1.0);
    tunnel_piece(inx, iny, inz, inpoints, true); //inside tunnel
    glColor4f(.8, .6, .5, 1.0);
    glBegin(GL_TRIANGLE_STRIP); //end of tunnel
    glNormal3f(0, 0, -1);
        for (int i = 0; i < inpoints; i++) {
            glVertex3f(stoutx[i], stouty[i], 0);
            glVertex3f(stinx[i], stiny[i], 0);
        }
        glVertex3f(stoutx[15], stouty[15], 0);
        glVertex3f(stoutx[16], stouty[16], 0);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP); //end of tunnel
        glNormal3f(1, 0, 0);
        for (int i = 0; i < inpoints; i++) {
            glVertex3f(outx[i], outy[i], outz[i]);
            glVertex3f(inx[i], iny[i], inz[i]);
        }
        glVertex3f(outx[15], outy[15], outz[15]);
        glVertex3f(outx[16], outy[16], outz[16]);
    glEnd();
}

//train station with textures
void station()
{
    glEnable(GL_TEXTURE_2D);
    //base coordinates for 2 halves
    float f1z[13] = {20, -8.579, -7.342, -5.811, -4.456, -3.278, -2.279, -1.46, -0.822, -0.365, -0.091, 0, 20};
    float f1x[13] = {-50, -50, -46.353, -41.346, -36.288, -31.187, -26.047, -20.876, -15.679, -10.463, -5.235, 0, 0};
    float f2z[13] = {20, 20, 0, -0.091, -0.365, -0.822, -1.46, -2.279, -3.278, -4.456, -5.811, -7.342, -8.579};
    float f2x[13] = {50, 0, 0, 5.235, 10.463, 15.679, 20.876, 26.047, 31.187, 36.288, 41.346, 46.353, 50};
    //roof
    glColor4f(1, 0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);
        normal(50, 20, -5, -50, 20, -5, 50, 27, 7.5);
        glTexCoord2f(0, 0); glVertex3f(-50, 20, -5);
        glTexCoord2f(2, 0); glVertex3f(50, 20, -5);
        glTexCoord2f(2, 1); glVertex3f(50, 27, 7.5);
        glTexCoord2f(0, 1); glVertex3f(-50, 27, 7.5);
    glEnd();
    glBegin(GL_QUADS);
        normal(-50, 20, 20, 50, 20, 20, -50, 27, 7.5);
        glTexCoord2f(0, 0); glVertex3f(50, 20, 20);
        glTexCoord2f(2, 0); glVertex3f(-50, 20, 20);
        glTexCoord2f(2, 1); glVertex3f(-50, 27, 7.5);
        glTexCoord2f(0, 1); glVertex3f(50, 27, 7.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //other roof bits - sides and bottom
    glColor4f(1, 1, 0.5, 1.0);
    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(-49, 20, -5);
        glVertex3f(49, 20, -5);
        glVertex3f(49, 20, 20);
        glVertex3f(-49, 20, 20);
    glEnd();
    glBegin(GL_TRIANGLES);
        glNormal3f(-1, 0, 0);
        glVertex3f(-49, 20, -5);
        glVertex3f(-49, 27, 7.5);
        glVertex3f(-49, 20, 20);
    glEnd();
    glBegin(GL_TRIANGLES);
        glNormal3f(1, 0, 0);
        glVertex3f(49, 20, -5);
        glVertex3f(49, 27, 7.5);
        glVertex3f(49, 20, 20);
    glEnd();
    //base top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glColor4f(0.8, 0.8, 0.8, 1.0);
    glNormal3f(0, 1, 0);
    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 1); glVertex3f(f1x[0], 6, f1z[0]);
        for (int i = 1; i < 12; i++) {
            glTexCoord2f((float)(i-1)/11, 0); glVertex3f(f1x[i], 6, f1z[i]);
        }
        glTexCoord2f(1, 1); glVertex3f(f1x[12], 6, f1z[12]);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(1, 1);  glVertex3f(f2x[0], 6, f2z[0]);
        glTexCoord2f(0, 1);  glVertex3f(f2x[1], 6, f2z[1]);
        for (int i = 2; i < 13; i++) {
            glTexCoord2f((float)(i-1)/12, 0); glVertex3f(f2x[i], 6, f2z[i]);
        }
    glEnd();
    //base front
    glBegin(GL_QUAD_STRIP);
        for (int i = 1; i < 12; i++) {
            if (i==11) {
                normal(f1x[10], 0, f1z[10], f1x[10], 6, f1z[10], f1x[i], 0, f1z[i]);
            } else {
                normal(f1x[i], 0, f1z[i], f1x[i], 6, f1z[i], f1x[i+1], 0, f1z[i+1]);
            }
            glTexCoord2f(0, (float)(i-1)/11); glVertex3f(f1x[i], 0, f1z[i]);
            glTexCoord2f(0.5, (float)(i-1)/11); glVertex3f(f1x[i], 6, f1z[i]);
        }
        for (int i = 2; i < 13; i++) {
            if (i==12) {
                normal(f2x[11], 0, f2z[11], f2x[11], 6, f2z[11], f2x[i], 0, f2z[i]);
            } else {
                normal(f2x[i], 0, f2z[i], f2x[i], 6, f2z[i], f2x[i+1], 0, f2z[i+1]);
            }
            glTexCoord2f(0, (float)(i-1)/12); glVertex3f(f2x[i], 0, f2z[i]);
            glTexCoord2f(0.5, (float)(i-1)/12); glVertex3f(f2x[i], 6, f2z[i]);
        }

    glEnd();
    //base sides
    glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(-50, 0, -8.579);
        glTexCoord2f(1, 1); glVertex3f(-50, 6, -8.579);
        glTexCoord2f(0, 1); glVertex3f(-50, 6, 20);
        glTexCoord2f(0, 0); glVertex3f(-50, 0, 20);
    glEnd();
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(1, 0); glVertex3f(-50, 0, 20);
        glTexCoord2f(1, 1); glVertex3f(-50, 6, 20);
        glTexCoord2f(0, 1); glVertex3f(50, 6, 20);
        glTexCoord2f(0, 0); glVertex3f(50, 0, 20);
    glEnd();
    glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(50, 0, 20);
        glTexCoord2f(1, 1); glVertex3f(50, 6, 20);
        glTexCoord2f(0, 1); glVertex3f(50, 6, -8.579);
        glTexCoord2f(0, 0); glVertex3f(50, 0, -8.579);
    glEnd();
    //pillars
    glColor4f(0.8, 0.8, 0.8, 1.0);
    GLUquadric *q = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    gluQuadricTexture(q, true);
    glPushMatrix();
        glTranslatef(-40, 6, 7.5);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(q, 2, 2, 14, 20, 4);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(40, 6, 7.5);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(q, 2, 2, 14, 20, 4);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//tree
void tree()
{
    glColor4f(0.4, 0.2, 0.0, 1.0); //brown trunk
    GLUquadric *q = gluNewQuadric();
    glPushMatrix();
        glRotatef(-90, 1, 0.0, 0);
        gluCylinder(q, 0.4, 0.4, 2, 20, 4);
	glPopMatrix();

    glColor4f(0.0, 0.3, 0.0, 1.0); //green leaves
    glPushMatrix();
        glTranslatef(0.0, 1, 0.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(2, 6, 20, 4);
	glPopMatrix();
}

//pond decoration
void pond()
{
    glTranslatef(0, 0.1, 0);
    glPushMatrix();
        glTranslatef(45, 0, 10);
        glScalef(3, 3, 3);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(42, 0, -5);
        glScalef(2, 2, 2);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(50, 0, -2);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(35, 0, 18);
        tree();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-30, 0, -30);
        glScalef(2, 2, 2);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-25, 0, -38);
        glScalef(1.5, 1.5, 1.5);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-35, 0, -37);
        tree();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-30, 0, 33);
        glScalef(2, 2, 2);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-40, 0, 30);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-20, 0, 35);
        tree();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-70, 0, -10);
        glScalef(2, 2, 2);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-65, 0, -18);
        tree();
    glPopMatrix();

    GLUquadric *q = gluNewQuadric(); //Disc
    glColor4f(0.0, 0.3, 0.6, 1.0); //blue pond
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluDisk(q, 0.0, 35.0, 36, 10);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-40, 0, 0);
        glRotatef(-90, 1, 0, 0);
        gluDisk(q, 0.0, 25.0, 36, 10);
    glPopMatrix();
}


//windmill blade with texture
void blade()
{
    glColor4f(0.5, 0.3, 0.1, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glNormal3f(0, 0, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(5, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(5, 1, 0);
        glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
    glEnd();
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(5, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(30, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(30, 7, 0);
        glTexCoord2f(0, 1); glVertex3f(5, 7, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//windmill with textures
void windmill(int rotate)
{
    glEnable(GL_TEXTURE_2D); //concrete building
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    GLUquadric *q = gluNewQuadric();
    gluQuadricTexture(q, true);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
        glRotatef(-90, 1, 0.0, 0);
        gluCylinder(q, 18, 15, 40, 30, 7);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glColor4f(0.45, 0.45, 0.4, 1.0); //top
    glPushMatrix();
        glTranslatef(0.0, 40, 0.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(15, 10, 30, 4);
    glPopMatrix();

    glColor4f(0.5, 0.3, 0.1, 1.0); //connector
    glPushMatrix();
        glTranslatef(-.5, 37, 15);
        gluCylinder(q, 1, 1, 3, 8, 2);
    glPopMatrix();

    glTranslatef(-.5, 37, 18); //four rotating blades
    glRotatef(-rotate, 0, 0, 1);
    for (int i = 0; i < 4;i++) {
        glRotatef(-i*90, 0, 0, 1);
        blade();
    }
}

//signal light
void signal(bool on)
{
    glPushMatrix(); //green when on
        if (on == true) {
            glColor4f(0, 0.8, 0, 1.0);
        } else {
            glColor4f(0.2, 0.2, 0.2, 1.0);
        }
        glTranslatef(0.0, 14.5, .5);
        glutSolidSphere(1, 22, 10);
    glPopMatrix();

    glPushMatrix(); //red when off
        if (on == false) {
            glColor4f(0.8, 0, 0, 1.0);
        } else {
            glColor4f(0.2, 0.2, 0.2, 1.0);
        }
        glTranslatef(0.0, 12.5, .5);
        glutSolidSphere(1, 22, 10);
    glPopMatrix();

    //structure
    glColor4f(0.3, 0.3, 0.3, 1.0);
    GLUquadric *q = gluNewQuadric();
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluCylinder(q, 0.5, 0.5, 11, 20, 4);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(0.0, 13.5, 0.0);
      glScalef(3, 5, 2);
      glutSolidCube(1.0);
    glPopMatrix();
}
