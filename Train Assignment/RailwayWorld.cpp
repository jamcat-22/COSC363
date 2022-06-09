#include <iostream>
#include <fstream>
#include <climits>
#include <math.h>
#include <GL/freeglut.h>
#include "RailModels.h"
using namespace std;

float *x,*z; //track coords
int npoints, counter=0; //track points, counter
float cam_angle=0, eye_x=0, cam_y=30, eye_z=0, look_x, look_y=20, look_z=-100;  //Camera parameters
int iccur = 0, camera = 0, rotate = 0; // counting, cam num, windmill
bool on = true; //signal light
float lookx, lookz, camx, camz; //other cam values

//load file for track
void loadFile(const char* fname)
{
    ifstream fp_in;

    fp_in.open(fname, ios::in);
    if(!fp_in.is_open())
    {
        cout << "Error opening file" << endl;
        exit(1);
    }

    fp_in >> npoints;

    x = new float[npoints];
    z = new float[npoints];

    for(int i=0; i < npoints; i++)
        fp_in >> x[i] >> z[i];

    fp_in.close();
    cout << " File successfully read." << endl;
}

void initialize(void) 
{
    loadFile("trackcoords.txt");

    loadTexture();
    glEnable(GL_TEXTURE_2D);

    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};
    float black[4]  = {0.0, 0.0, 0.0, 1.0};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf(GL_FRONT, GL_SHININESS, 90);

    //Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 3);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    glClearColor (0.6, 0.8, 0.9, 0.0);  //Background colour

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(50., 1.0, 10.0, 1000.0);   //Perspective projection
}

//-------------------------------------------------------------------
void display(void)
{
   float lgt_pos[] = {0.0f, 100.0f, 0.0f, 1.0f};  //light0 position (directly above the origin)
   //spotlight
   float lgt_pos1[] = {-17.0f, 6.0f, 0.0f, 1.0f};
   float spot_pos1[] = {-1.0f, -1.0f, 0.0f};

   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   //change camera view
   if (camera == 0) {
       gluLookAt(eye_x, cam_y, eye_z, look_x, look_y, look_z, 0, 1, 0);
   } else if (camera == 1) {
       gluLookAt(48, 15, 330, -134, 1, 217, 0, 1, 0);
   } else if (camera == 2) {
       gluLookAt(camx, 15, camz, lookx, 10, lookz, 0, 1, 0);
   }

   glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos); //place light

   floor(); //floor
   tracks(npoints, x, z); //tracks
   sleepers(npoints, x, z); //sleepers
   //station
   glPushMatrix();
       glTranslatef(0, 0, 310);
       station();
   glPopMatrix();
   //tunnel
   glPushMatrix();
       glTranslatef(105.6, 0, -256.5);
       glRotatef(-136, 0, 1, 0);
       tunnel();
   glPopMatrix();
   //windmill
   glPushMatrix();
       glTranslatef(50, 0, -170);
       windmill(rotate);
   glPopMatrix();
   //signal lights
   glPushMatrix();
       glTranslatef(95, 0, 250);
       glRotatef(-45, 0, 1, 0);
       signal(on);
   glPopMatrix();

   //just some pretty decorations
   glPushMatrix();
       glTranslatef(25, 0, 150);
       glScalef(2, 2, 2);
       pond();
   glPopMatrix();
   glPushMatrix();
       glTranslatef(-35, 0, -130);
       glRotatef(110, 0, 1, 0);
       pond();
   glPopMatrix();

   //train movements
   std::pair<float, float> p0;
   std::pair<float, float> p1;
   std::pair<float, float> u0;
   int models = 4; //number of train models
   int m = 6; //distance between trains
   for (int i=0; i < models; i++) {
       p0 = {x[(iccur + i*m) % npoints], z[(iccur + i*m) % npoints]};
       p1 = {x[((iccur + i*m) +1) % npoints], z[((iccur + i*m) +1) % npoints]};
       u0 = {(p1.first - p0.first), (p1.second - p0.second)};
       normal_2d(u0);
       glPushMatrix();
           glTranslatef(p0.first, 1, p0.second);
           glRotatef(atan2(u0.second, -u0.first)*180/M_PI, 0, 1, 0);
           if (i == 3) {
               camx = x[((iccur + i*m) +2) % npoints];
               camz = z[((iccur + i*m) +2) % npoints];
               lookx = x[((iccur + i*m) +10) % npoints];
               lookz = z[((iccur + i*m) +10) % npoints];
               glLightfv(GL_LIGHT1, GL_POSITION, lgt_pos1);
               glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_pos1);
               engine();
           } else if (i == 2) {
               wagon();
           } else if (i == 1) {
               wagon();
           } else if (i == 0) {
               wagon();
           }
       glPopMatrix();
   }

   glutSwapBuffers();   //Useful for animation
}

void myTimer(int value)
{
    //to stop the train
    if (counter < 354 || counter >= 384) {
        iccur++;
        iccur = iccur % npoints;
    }
    //change the lights
    if (counter < 330 || counter >= 380) {
        on = true;
    } else {
        on = false;
    }
    counter++;
    counter = counter % (npoints+30);
    rotate+=2; //windmill
    rotate = rotate % 360;
    glutPostRedisplay();
    glutTimerFunc(80, myTimer, 0);
}

void special(int key, int x, int y)
{
    //cam height
    if (key == GLUT_KEY_PAGE_UP && cam_y < 400) {
        cam_y++;
        look_y++;
    } else if (key == GLUT_KEY_PAGE_DOWN && cam_y > 20) {
        cam_y--;
        look_y--;
    }
    if(key == GLUT_KEY_LEFT) cam_angle -= 0.1;//cam left
    else if(key == GLUT_KEY_RIGHT) cam_angle += 0.1;//cam right
    else if(key == GLUT_KEY_DOWN) {  //Move backward
        eye_x -= 1.5*sin(cam_angle);
        eye_z += 1.5*cos(cam_angle);
    } else if(key == GLUT_KEY_UP) { //Move forward
        eye_x += 1.5*sin(cam_angle);
        eye_z -= 1.5*cos(cam_angle);
    }
    look_x = eye_x + 40*sin(cam_angle);
    look_z = eye_z - 40*cos(cam_angle);
    glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
    //change cam view
    if (key == 'c') {
        camera++;
        camera = camera % 3;
    }
}

//---------------------------------------------------------------------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH);
   glutInitWindowSize (600, 600); 
   glutInitWindowPosition (50, 50);
   glutCreateWindow ("Train");
   glutSpecialFunc(special);
   glutKeyboardFunc(keys);
   initialize ();
   glutTimerFunc(50, myTimer, 0);
   glutDisplayFunc(display); 
   glutMainLoop();
   return 0;
}
