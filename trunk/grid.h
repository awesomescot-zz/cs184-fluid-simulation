#include <iostream>
#include "algebra3.h"

#ifndef _grid_h
#define _grid_h

using namespace std;

struct cube{
	float u, v, w, temp;
};



class grid {
public:
	cube ***cubeGrid;
	float x, y, z;
	int xSplit, ySplit, zSplit;
	float xCubeSize, yCubeSize, zCubeSize;
	grid(){};
	grid(float,float, float, int, int, int);
	cube getCube(int, int, int);
	cube getCube(vec3);
	vec3 getVelosity(vec3);
};

#endif
