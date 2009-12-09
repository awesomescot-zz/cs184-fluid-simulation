
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include "algebra3.h"
//#include "grid.h"

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

using namespace std;
float t;

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
			particles[i] = particles[i] + velocity * deltaT;
		}
	}

	void particlesLocs() {
		for (int i = 0; i < (int)particles.size(); i++) {
			cout << i << ": " << particles[i] << endl;
		}
	}
};

Viewport viewport;

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

void initScene() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

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

void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, -2, 0, 0, 0, 0, 1, 0);

	// before drawing, update new particle locations
	viewport.update();

	// start drawing here
	glColor3f(1.0f, 1.0f, 1.0f);

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

	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myFrameMove);
	glutMainLoop();

	return 0;
}
