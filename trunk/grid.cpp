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

/*
 * things to do:
 * 1) velocity advection.  for every velocity vector trace it back wards delta T, and reset it to that velocity.
 * 2) smoothing.  for every cube determine its presure.  if it's not zero we must change velocities so that it will reach zero.
 * There are coeficients that we use to know how much we need to change the velocities by.
 * 3) move particles.  Particles move forward as you think they would.
 *
 * velocity advection(){
 *   for every velocity{
 *     calculate xyz vector for that side
 *     move negative xyz*delta T in that direction
 *     calculate the velocity at that spot
 *     copy that velocity to the new grid at our original location(a sides velocity vector)
 *
 * smoothing:
 *   do over and over:
 *    for every cube:
 *     calc pressure, from vel vectors coming in and out
 *     use coeficients from the paper to determine how much to change the velocity vectors.
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
	int gridSpotz = location[2] / zCubeSize;
	return cubeGrid[gridSpotx][gridSpoty][gridSpotz];
}

vec3 grid::getVelosity(vec3 location){
	int gridSpotx = location[0] / xCubeSize;
	int gridSpoty = location[1] / yCubeSize;
	int gridSpotz = location[2] / zCubeSize;

	if( (gridSpotx*xCubeSize) + .5*xCubeSize < location[0]){//if the point is over half way through the cube in the x direction
		float xdistBack = location[0] - (gridSpotx*xCubeSize) + .5*xCubeSize;
		float xdistForward = (gridSpotx+1)*xCubeSize + .5*xCubeSize - location[0];
		float zdistBack = location[2] - (gridSpotz*zCubeSize) + .5*zCubeSize;
		float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
		float rightSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w + (xdistForward/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].w;
		float leftSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w + (xdistForward/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].w;
		float wfullAverage = (zdistBack/zCubeSize)*leftSideAverage + (zdistForward/zCubeSize)*rightSideAverage;
	}else{
		float xdistBack = location[0] - (gridSpotx-1)*xCubeSize + .5*xCubeSize;
		float xdistForward = (gridSpotx)*xCubeSize + .5*xCubeSize - location[0];
		float zdistBack = location[2] - (gridSpotz*zCubeSize) + .5*zCubeSize;
		float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
		float rightSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz].w + (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w;
		float leftSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz+1].w + (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w;
		float wfullAverage = (zdistBack/zCubeSize)*leftSideAverage + (zdistForward/zCubeSize)*rightSideAverage;
	}
}
