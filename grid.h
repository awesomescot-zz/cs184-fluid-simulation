#include <iostream>
#include "algebra3.h"

#ifndef _grid_h
#define _grid_h

using namespace std;

struct cube{
	float u, v, w, temp, comp;
};

class grid {
public:
	cube ***cubeGrid;
	float x, y, z;
	int xSplit, ySplit, zSplit;
	float xCubeSize, yCubeSize, zCubeSize;
	grid(){};
	grid(vec3, vec3, vec3, vec3, int, int, int);
	cube getCube(int, int, int);
	cube getCube(vec3);
	vec3 getVelosity(vec3);
};

#endif
