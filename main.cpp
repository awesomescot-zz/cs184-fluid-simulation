
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include "algebra3.h"
#include "grid.h"

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

#ifdef OSX
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/glut.h>
	#include <GL/glu.h>
#endif

float deltaT = 1;

// temporary velocity updates for keyboard interaction
float vx = 0;
float vy = 0;
float vz = 0;

using namespace std;

class Viewport;

class Viewport {
private:
	vector<vec3> particles;

public:
	int w, h;

	void addParticle(vec3 loc) {
		particles.push_back(loc);
	}

	vec3 getLoc(int i) {
		return particles[i];
	}

	int numParticles() {
		return particles.size();
	}

	void sort() {
		for (int i = 0; i < (int)(particles.size() - 1); i++) {
			for (int j = 0; j < (int)(particles.size() - 1); j++) {
				if (particles[j][2] < particles[j+1][2]) {
					vec3 temp = particles[j];
					particles[j] = particles[j+1];
					particles[j+1] = temp;
				}
			}
		}
	}

	void update() {

		// replace this with vec3 velocity = myGrid.getVelocity();
		vec3 velocity = vec3(-1.0/30, -0.3/30, 1.0/30);

		for (int i = 0; i < (int)particles.size(); i++) {
			particles[i] = particles[i] + (velocity + vec3(vx, vy, vz)) * deltaT;
		}
	}

	void particlesLocs() {
		for (int i = 0; i < (int)particles.size(); i++) {
			cout << i << ": " << particles[i] << endl;
		}
	}
};

Viewport viewport;
grid grd;
/*
void advection() {
	/*
	 * velocity advection(){
	 *   for every velocity
	 *     calculate xyz vector for that side
	 *     move negative xyz*delta T in that direction
	 *     calculate the velocity at that spot
	 *     copy that velocity to the new grid at our original location(a sides velocity vector)
	 *
	 * getVelosity on one of the face centers
	 * go backwards along velocity vector by delta T time
	 * getVelosity at this previous point
	 * take the appropriate part of the previous vector, put into a blank copy of the grid
	 * repeat for every center of every face of every cube in the grid
	 * done
	 * no particular timestep length or speed of velocity yet
	 *
	// grd = our global grid
	grid newGrid = grid(grd.x, grd.y, grd.z, grd.xSplit, grd.ySplit, grd.zSplit);
	vec3 vel, point;

	// xi, yi, zi = indices for the cubes in the grid
	for (float xi = 0; xi < grd.x; xi++) { // < or <= ? depends on how to handle border cases
		for (float yi = 0; yi < grd.y; yi++) {
			for (float zi = 0; zi < grd.z; zi++) {
				// copy old grid info into new grid while we're at it
				newGrid[xi][yi][zi].temp = grd[xi][yi][zi].temp;
				newGrid[xi][yi][zi].comp = grd[xi][yi][zi].comp;

				// iterate through the x, y, and z faces (other three faces handled by other cubes)
				// take point on face, get velocity, move back by (vel * deltaT), get new velocity, update on new grid
				// TODO: what happens if we end up outside of the grid?

				// x/u face
				// multiply by cube size to get correct input (want point in the grid, not cube index) for getVelosity
				point = vec3(xi * grd.xCubeSize, yi * grd.yCubeSize + (grd.yCubesize)/2, zi * grd.zCubeSize + (grd.zCubeSize)/2);
				vel = getVelosity(point);
				point = point - (vel * deltaT);
				vel = getVelosity(point);
				newGrid[xi][yi][zi].u = vel[0];

				// y/v face
				// multiply by cube size to get correct input (want point in the grid, not cube index) for getVelosity
				point = vec3(xi * grd.xCubeSize + (grd.xCubeSize)/2, yi * grd.yCubeSize, zi * grd.zCubeSize + (grd.zCubeSize)/2);
				vel = getVelosity(point);
				point = point - (vel * deltaT);
				vel = getVelosity(point);
				newGrid[xi][yi][zi].v = vel[1];

				// z/w face
				// multiply by cube size to get correct input (want point in the grid, not cube index) for getVelosity
				point = vec3(xi * grd.xCubeSize + (grd.xCubeSize)/2, yi * grd.yCubeSize + (grd.yCubesize)/2, zi * grd.zCubeSize);
				vel = getVelosity(point);
				point = point - (vel * deltaT);
				vel = getVelosity(point);
				newGrid[xi][yi][zi].w = vel[2];
			}
		}
	}

	grd = newGrid;
}
*/
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	GLfloat ratio = (GLfloat)w/(GLfloat)h;

	glViewport(0, 0, viewport.w, viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-2, 2, -2, 2, 1, 100);
	gluPerspective(45, ratio, 1, 100);
}

//temporary grid data
	int gx = -1;
	int gy = 1;
	int gz = 1;
	int xSplit = 5;
	int ySplit = 5;
	int zSplit = 5;

void initScene() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	// create grid
	//grd = grid(-1, 1, 1, 5, 5, 5);

	// 5 random particles
	viewport.addParticle(vec3(1, 1, 1));
	viewport.addParticle(vec3(0.0, 0.0, 5));
	viewport.addParticle(vec3(0.7, 1, 4));
	viewport.addParticle(vec3(0.5, -0.7, 4));
	viewport.addParticle(vec3(-0.5, -0.3, 9));

	myReshape(viewport.w, viewport.h);
}

void initLights() {
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	//define and enable light 0
	GLfloat ambient[] = {0.1f, 0.1f, 0.1f};
	GLfloat diffuse[] = {0.6f, 0.5f, 0.5f};
	GLfloat specular[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat pos[] = {-3, 0, 2, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_LIGHT0);

	GLfloat ambient1[] = {0.1f, 0.1f, 0.1f};
	GLfloat diffuse1[] = {0.5f, 0.5f, 0.5f};
	GLfloat specular1[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat pos1[] = {3, 0, -2, 1};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	glEnable(GL_LIGHT1);

	GLfloat mat_specular[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat mat_diffuse[] = {0.0, 0.8, 0.5, 1.0};
	GLfloat mat_ambient[] = {0.0, 0.1, 0.1, 1.0};
	//GLfloat mat_emission[] = {0.2, 0.0, 0.0, 1.0};
	GLfloat mat_shininess = {10.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	glEnable(GL_NORMALIZE);
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27 :
			exit(0);
		case 'r' :
			initScene();
			break;
	}
}

void processInputKeys(int key, int x, int y) {

	int mod = glutGetModifiers();
	switch(key) {
		case GLUT_KEY_LEFT :
			vx = vx + (float)1/30;
			break;
		case GLUT_KEY_RIGHT :
			vx = vx - (float)1/30;
			break;
		case GLUT_KEY_UP :
			if (mod == GLUT_ACTIVE_ALT)
				vz = vz - (float)1/30;
			else
				vy = vy + (float)1/30;
			break;
		case GLUT_KEY_DOWN :
			if (mod == GLUT_ACTIVE_ALT)
				vz = vz + (float)1/30;
			else
				vy = vy - (float)1/30;
			break;
	}
}

void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(gx, gy, -2, 0, 0, gz, 0, 1, 0);
	gluLookAt(0, 0, -2, 0, 0, 0, 0, 1, 0);

	// before drawing, update new particle locations
	viewport.update();

	// start drawing here
	glColor3f(1.0f, 1.0f, 1.0f);

	//draw grid
/*	//z-axis aligned
	glBegin(GL_LINES);
	float xs = (float)gx/xSplit;
	float ys = (float)gy/ySplit;
	float zs = (float)gz/zSplit;
	
	for (int i = 0; i < xSplit + 1; i++) {
		for (int j = 0; j < ySplit + 1; j++) {
			glVertex3f(xs*i, ys*j, 0);
			glVertex3f(xs*i, ys*j, gz);
		}
	}
	//y-axis aligned
	for (int i = 0; i < xSplit + 1; i++) {
		for (int k = 0; k < zSplit + 1; k++) {
			glVertex3f(xs*i, 0, zs*k);
			glVertex3f(xs*i, gy, zs*k);
		}
	}
	//x-axis aligned
	for (int j = 0; j < ySplit + 1; j++) {
		for (int k = 0; k < zSplit + 1; k++) {
			glVertex3f(0, ys*j, zs*k);
			glVertex3f(gx, ys*j, zs*k);
		}
	}
	glEnd();*/

	//z-axis aligned
	glBegin(GL_LINES);
	float xs = (float)grd.x/grd.xSplit;
	float ys = (float)grd.y/grd.ySplit;
	float zs = (float)grd.z/grd.zSplit;
	
	for (int i = 0; i < grd.xSplit + 1; i++) {
		for (int j = 0; j < grd.ySplit + 1; j++) {
			glVertex3f(xs*i, ys*j, 0);
			glVertex3f(xs*i, ys*j, grd.z);
		}
	}
	//y-axis aligned
	for (int i = 0; i < grd.xSplit + 1; i++) {
		for (int k = 0; k < grd.zSplit + 1; k++) {
			glVertex3f(xs*i, 0, zs*k);
			glVertex3f(xs*i, grd.y, zs*k);
		}
	}
	//x-axis aligned
	for (int j = 0; j < grd.ySplit + 1; j++) {
		for (int k = 0; k < grd.zSplit + 1; k++) {
			glVertex3f(0, ys*j, zs*k);
			glVertex3f(grd.x, ys*j, zs*k);
		}
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// draw particles
	for (int i = 0; i < viewport.numParticles(); i++){

		vec3 part = viewport.getLoc(i);

		glPushMatrix();
			glTranslatef(part[0], part[1], part[2]);
			glutSolidSphere(0.25, 50, 50);			// glutSolidSphere(radius, splice(long), splices(lat))
		glPopMatrix();
	}

	glutSwapBuffers();
}

void myFrameMove() {
	//nothing here for now
#ifdef _WIN32
	Sleep(10);				//give ~10ms back to OS
#endif
	glutPostRedisplay();	//force glut to call display
}

int main(int argc, char *argv[]) {
	//initialize glut
	glutInit(&argc, argv);

	//double-buffered window with red, green, blue
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	//initialize viewport size
	viewport.w = 400;
	viewport.h = 400;

	//size and position of window
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Particle Simulation");

	//initialize scene
	initScene();
	initLights();

	//keyboard interaction
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processInputKeys);

	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myFrameMove);
	glutMainLoop();

	return 0;
}
