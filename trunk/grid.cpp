#include <iostream>
#include "algebra3.h"
#include "grid.h"

/*
struct cube{
	float u, v, w, temp, comp;
	vec3 x, y, z, start;
};

class grid {
public:
	vec3 base, x, y, z;
	int xSplit, ySplit, zSplit;
	vec3 xCubeSize, yCubeSize, zCubeSize;
	grid(){};
	grid(int, vec3, vec3, vec3, vec3, int, int, int);
	cube getCube(int, int, int);
	vec3 getVelosity(vec3);
};
*/

grid::grid(float X, float Y, float Z, int xs, int ys, int zs){
	x = X;
	y = Y;
	z = Z;
	xSplit = xs;
	ySplit = ys;
	zSplit = zs;
	xCubeSize = x / xSplit;
	yCubeSize = y / ySplit;
	zCubeSize = z / zSplit;
	for(int i=0; i<xSplit; i++){
		for(int j=0; j<ySplit; j++){
			for(int k=0; k<zSplit; k++){
				cubeGrid[i][j][k] = new cube();
				cubeGrid[i][j][k].u = 0;
				cubeGrid[i][j][k].v = 0;
				cubeGrid[i][j][k].w = 0;
				cubeGrid[i][j][k].temp = 0;
				cubeGrid[i][j][k].comp = 1;
			}
		}
	}
}

cube grid::getCube(int a, int b, int c){
	return cubeGrid[a][b][c];
}

cube grid::getCube(vec3 location){
	int gridSpotx = location[0] / xCubeSize;
	int gridSpoty = location[1] / yCubeSize;
	int gridSpotx = location[0] / xCubeSize;
	return cubeGrid[gridSpotx][gridSpoty][gridSpotz];
}

vec3 grid::getVelosity(vec3 location){


}
