#include <iostream>
#include "algebra3.h"
#include "grid.h"



/*
struct cube{
	float u, v, w, temp;
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
	cubeGrid = new cube** [xSplit];
	for(int i=0; i<xSplit; i++){
		cubeGrid[i] = new cube* [ySplit];
		for(int j=0; j<ySplit; j++){
			cubeGrid[i][j] = new cube[zSplit];
			for(int k=0; k<zSplit; k++){
				cubeGrid[i][j][k] = cube();
				cubeGrid[i][j][k].u = 0;
				cubeGrid[i][j][k].v = 0;
				cubeGrid[i][j][k].w = 0;
				cubeGrid[i][j][k].temp = 0;
			}
		}
	}
}

cube grid::getCube(int a, int b, int c){
	return cubeGrid[a][b][c];
}

void grid::print(){
	for(int i=0; i<xSplit; i++){
		for(int j=0; j<ySplit; j++){
			for(int k=0; k<zSplit; k++){
				printf("cubeGrid[%d][%d][%d]   U = %f,  V = %f,  W = %f\n", i,j,k,cubeGrid[i][j][k].u,cubeGrid[i][j][k].v,cubeGrid[i][j][k].w);
			}
		}
	}
}

cube grid::getCube(vec3 location){
	int gridSpotx = location[0] / xCubeSize;
	int gridSpoty = location[1] / yCubeSize;
	int gridSpotz = location[2] / zCubeSize;
	return cubeGrid[gridSpotx][gridSpoty][gridSpotz];
}

vec3 grid::getVelosity(vec3 location){
	//check for out of bounds locations
	//printf("x = %f, y = %f, z=%f",x,y,z);
	if(location[0] >= x || location[0] < 0) return vec3(0,0,0);
	if(location[1] >= y || location[1] < 0) return vec3(0,0,0);
	if(location[2] >= z || location[2] < 0) return vec3(0,0,0);
	//cout << "here" << endl;

	int gridSpotx = location[0] / xCubeSize;
	int gridSpoty = location[1] / yCubeSize;
	int gridSpotz = location[2] / zCubeSize;
	// float secondRightSideAverage, secondLeftSideAverage, secondTopSideAverage, secondBottomSideAverage, secondCloseSideAverage, secondFarSideAverage, fullFarSideAverage, firstCloseSideAverage, firstFarSideAverage, fullCloseSideAverage, firstRightSideAverage, firstLeftSideAverage, firstBottomSideAverage, firstTopSideAverage, fullRightSideAverage, fullLeftSideAverage, fullTopSideAverage, fullBottomSideAverage, ufullAverage, vfullAverage, wfullAverage;
	float secondRightSideAverage = 0;
	float secondLeftSideAverage = 0;
	float secondTopSideAverage = 0;
	float secondBottomSideAverage = 0;
	float secondCloseSideAverage = 0;
	float secondFarSideAverage = 0;
	float fullFarSideAverage = 0;
	float firstCloseSideAverage = 0;
	float firstFarSideAverage = 0;
	float fullCloseSideAverage = 0;
	float firstRightSideAverage = 0;
	float firstLeftSideAverage = 0;
	float firstBottomSideAverage = 0;
	float firstTopSideAverage = 0;
	float fullRightSideAverage = 0;
	float fullLeftSideAverage = 0;
	float fullTopSideAverage = 0;
	float fullBottomSideAverage = 0;
	float ufullAverage = 0;
	float vfullAverage = 0;
	float wfullAverage = 0;

	//W
	float zdistBack = location[2] - (gridSpotz*zCubeSize);
	float zdistForward = (gridSpotz+1)*zCubeSize - location[2];
	if( (gridSpotx*xCubeSize) + .5*xCubeSize < location[0]){//if the point is over half way through the cube in the x direction
		float xdistBack = location[0] - ((gridSpotx*xCubeSize) + .5*xCubeSize);
		float xdistForward = (gridSpotx+1)*xCubeSize + .5*xCubeSize - location[0];
		if(gridSpotx+1 >= xSplit){ //check x bounds
			firstCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w;
			if(gridSpotz+1 >= zSplit) firstFarSideAverage = 0; //check z bounds
			else firstFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w;
		} else {
			firstCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].w;
			if(gridSpotz+1 >= zSplit) firstFarSideAverage = 0;
			else firstFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].w;
		}
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - ((gridSpoty*yCubeSize) + .5*yCubeSize);
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpotx+1 >= xSplit){
				if(gridSpoty+1 >= ySplit){
					secondCloseSideAverage = 0;
					secondFarSideAverage = 0;
				}else{
					secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].w;
					if(gridSpotz+1 >= zSplit) secondFarSideAverage = 0;
					else secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].w;
				}
			} else {
				if(gridSpoty+1 < ySplit){
					secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].w;
					if(gridSpotz+1 < zSplit) secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				}else {
					secondCloseSideAverage = 0;
					secondFarSideAverage = 0;
				}
			}
			fullCloseSideAverage = (ydistForward/yCubeSize)*firstCloseSideAverage + (ydistBack/yCubeSize)*secondCloseSideAverage;
			fullFarSideAverage = (ydistForward/yCubeSize)*firstFarSideAverage + (ydistBack/yCubeSize)*secondFarSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - ((gridSpoty-1)*yCubeSize + .5*yCubeSize);
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty > 0){
				if(gridSpotx+1 < xSplit){
					secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].w;
					if(gridSpotz+1<zSplit) secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				} else {
					secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].w;
					if(gridSpotz+1<zSplit) secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				}
			}else{
				secondCloseSideAverage = 0;
				secondFarSideAverage = 0;
			}
			fullCloseSideAverage = (ydistBack/yCubeSize)*firstCloseSideAverage + (ydistForward/yCubeSize)*secondCloseSideAverage;
			fullFarSideAverage = (ydistBack/yCubeSize)*firstFarSideAverage + (ydistForward/yCubeSize)*secondFarSideAverage;
		}
		wfullAverage = (zdistBack/zCubeSize)*fullFarSideAverage + (zdistForward/zCubeSize)*fullCloseSideAverage;
	}else{
		float xdistBack = location[0] - ((gridSpotx-1)*xCubeSize + .5*xCubeSize);
		float xdistForward = (gridSpotx)*xCubeSize + .5*xCubeSize - location[0];
		if(gridSpotx > 0){
			firstCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w;
			if(gridSpotz+1 < zSplit) firstFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w;
			else firstFarSideAverage = 0;
		}else {
			firstCloseSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w;
			if(gridSpotz+1 < zSplit) firstFarSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w;
			else firstFarSideAverage = 0;
		}
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - ((gridSpoty*yCubeSize) + .5*yCubeSize);
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty+1 < ySplit){
				if(gridSpotx > 0){
					secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].w;
					if(gridSpotz+1 < zSplit) secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				}else {
					secondCloseSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].w;
					if(gridSpotz+1 < zSplit) secondFarSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				}
			}else{
				secondCloseSideAverage = 0;
				secondFarSideAverage = 0;
			}
			fullCloseSideAverage = (ydistForward/yCubeSize)*firstCloseSideAverage + (ydistBack/yCubeSize)*secondCloseSideAverage;
			fullFarSideAverage = (ydistForward/yCubeSize)*firstFarSideAverage + (ydistBack/yCubeSize)*secondFarSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - ((gridSpoty-1)*yCubeSize + .5*yCubeSize);
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty > 0){
				if(gridSpotx > 0){
					secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty-1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].w;
					if(gridSpotz+1 < zSplit) secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty-1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				}else {
					secondCloseSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].w;
					if(gridSpotz+1 < zSplit) secondFarSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].w;
					else secondFarSideAverage = 0;
				}
			}else{
				secondCloseSideAverage = 0;
				secondFarSideAverage = 0;
			}
			fullCloseSideAverage = (ydistBack/yCubeSize)*firstCloseSideAverage + (ydistForward/yCubeSize)*secondCloseSideAverage;
			fullFarSideAverage = (ydistBack/yCubeSize)*firstFarSideAverage + (ydistForward/yCubeSize)*secondFarSideAverage;
		}
		wfullAverage = (zdistBack/zCubeSize)*fullFarSideAverage + (zdistForward/zCubeSize)*fullCloseSideAverage;

	}
	/*
	 *      _______
	 *     /|     /|
	 *    / |    / |
	 *   /  |   /  |
	 *  /   |  /   |
	 *  -------____|
	 *  |   / |   /
	 *  |  /  |  /
	 *  | /   | /
	 *  |/    |/
	 *  -------
	 */
	//U
	float xdistBack = location[0] - (gridSpotx*xCubeSize);
	float xdistForward = (gridSpotx+1)*xCubeSize - location[0];
	if( (gridSpotz*zCubeSize) + .5*zCubeSize < location[2]){//if the point is over half way through the cube in the z direction
		float zdistBack = location[2] - ((gridSpotz*zCubeSize) + .5*zCubeSize);
		float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
		if(gridSpoty+1 < zSplit){
			firstLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u + (zdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].u;
			if(gridSpotx+1<xSplit) firstRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].u;
			else firstRightSideAverage = 0;
		}
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - ((gridSpoty*yCubeSize) + .5*yCubeSize);
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty+1<ySplit){
				if(gridSpotz+1<zSplit){
					secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz+1].u;
					else secondRightSideAverage = 0;
				} else{
					secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].u;
					else secondRightSideAverage = 0;
				}
			}else {
				secondLeftSideAverage = 0;
				secondRightSideAverage = 0;
			}
			fullLeftSideAverage = (ydistForward/yCubeSize)*firstLeftSideAverage + (ydistBack/yCubeSize)*secondLeftSideAverage;
			fullRightSideAverage = (ydistForward/yCubeSize)*firstRightSideAverage + (ydistBack/yCubeSize)*secondRightSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - ((gridSpoty-1)*yCubeSize + .5*yCubeSize);
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty>0){
				if(gridSpotz+1<zSplit){
					secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz+1].u;
					else secondRightSideAverage = 0;
				}else{
					secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].u;
					else secondRightSideAverage = 0;
				}
			}else {
				secondLeftSideAverage = 0;
				secondRightSideAverage = 0;
			}
			fullLeftSideAverage = (ydistBack/yCubeSize)*firstLeftSideAverage + (ydistForward/yCubeSize)*secondLeftSideAverage;
			fullRightSideAverage = (ydistBack/yCubeSize)*firstRightSideAverage + (ydistForward/yCubeSize)*secondRightSideAverage;
		}
		ufullAverage = (xdistBack/xCubeSize)*fullRightSideAverage + (xdistForward/xCubeSize)*fullLeftSideAverage;
	}else{
		float zdistBack = location[2] - ((gridSpotz-1)*zCubeSize + .5*zCubeSize);
		float zdistForward = (gridSpotz)*zCubeSize + .5*zCubeSize - location[2];
		if(gridSpotx+1<xSplit){
			if(gridSpotz>0){
				firstLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u;
				firstRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].u;
			}else{
				firstLeftSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u;
				firstRightSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].u;
			}
		}else{
			firstRightSideAverage = 0;
			if(gridSpotz>0){
				firstLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u;
			}else{
				firstLeftSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u;
			}
		}

		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - ((gridSpoty*yCubeSize) + .5*yCubeSize);
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty+1<ySplit){
				if(gridSpotz>0){
					secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].u;
					else secondRightSideAverage = 0;
				}else{
					secondLeftSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].u;
					else secondRightSideAverage = 0;
				}
			}else{
				secondLeftSideAverage = 0;
				secondRightSideAverage = 0;
			}
			fullLeftSideAverage = (ydistForward/yCubeSize)*firstLeftSideAverage + (ydistBack/yCubeSize)*secondLeftSideAverage;
			fullRightSideAverage = (ydistForward/yCubeSize)*firstRightSideAverage + (ydistBack/yCubeSize)*secondRightSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - ((gridSpoty-1)*yCubeSize + .5*yCubeSize);
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			if(gridSpoty>0){
				if(gridSpotz>0){
					secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].u;
					else secondRightSideAverage = 0;
				}else{
					secondLeftSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].u;
					if(gridSpotx+1<xSplit) secondRightSideAverage = (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].u;
					else secondRightSideAverage = 0;
				}
			}else{
				secondLeftSideAverage = 0;
				secondRightSideAverage = 0;
			}
			fullLeftSideAverage = (ydistBack/yCubeSize)*firstLeftSideAverage + (ydistForward/yCubeSize)*secondLeftSideAverage;
			fullRightSideAverage = (ydistBack/yCubeSize)*firstRightSideAverage + (ydistForward/yCubeSize)*secondRightSideAverage;
		}
		ufullAverage = (xdistBack/xCubeSize)*fullRightSideAverage + (xdistForward/xCubeSize)*fullLeftSideAverage;
	}

	//V
	float ydistBack = location[1] - (gridSpoty*yCubeSize);
	float ydistForward = (gridSpoty+1)*yCubeSize - location[1];
	if( (gridSpotx*xCubeSize) + .5*xCubeSize < location[0]){//if the point is over half way through the cube in the x direction
		float xdistBack = location[0] - ((gridSpotx*xCubeSize) + .5*xCubeSize);
		float xdistForward = (gridSpotx+1)*xCubeSize + .5*xCubeSize - location[0];
		if(gridSpoty+1<ySplit){
			if(gridSpotx+1<xSplit){
				firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].v;
				firstTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].v;
			}else{
				firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v;
				firstTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].v;
			}
		}else{
			firstTopSideAverage = 0;
			if(gridSpotx+1<xSplit) firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].v;
			else firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v;
		}
		if((gridSpotz*zCubeSize) + .5*zCubeSize < location[2]){//over z half way point calculate for z+1
			float zdistBack = location[2] - ((gridSpotz*zCubeSize) + .5*zCubeSize);
			float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
			if(gridSpotz+1<zSplit){
				if(gridSpotx+1<xSplit){
					secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz+1].v;
					else secondTopSideAverage = 0;
				}else {
					secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].v;
					else secondTopSideAverage = 0;
				}
			}else {
				secondBottomSideAverage = 0;
				secondTopSideAverage = 0;
			}
			fullBottomSideAverage = (zdistForward/zCubeSize)*firstBottomSideAverage + (zdistBack/zCubeSize)*secondBottomSideAverage;
			fullTopSideAverage = (zdistForward/zCubeSize)*firstTopSideAverage + (zdistBack/zCubeSize)*secondTopSideAverage;
		} else{//calculate for y-1
			float zdistBack = location[2] - ((gridSpotz-1)*zCubeSize + .5*zCubeSize);
			float zdistForward = (gridSpotz)*zCubeSize + .5*zCubeSize - location[2];
			if(gridSpotz>0){
				if(gridSpotx+1<xSplit){
					secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz-1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz-1].v;
					else secondTopSideAverage = 0;
				}else{
					secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].v;
					else secondTopSideAverage = 0;
				}
			}else{
				secondBottomSideAverage = 0;
				secondTopSideAverage = 0;
			}
			fullBottomSideAverage = (zdistBack/zCubeSize)*firstBottomSideAverage + (zdistForward/zCubeSize)*secondBottomSideAverage;
			fullTopSideAverage = (zdistBack/zCubeSize)*firstTopSideAverage + (zdistForward/zCubeSize)*secondTopSideAverage;
		}
		vfullAverage = (ydistBack/yCubeSize)*fullTopSideAverage + (ydistForward/yCubeSize)*fullBottomSideAverage;
	}else{
		float xdistBack = location[0] - ((gridSpotx-1)*xCubeSize + .5*xCubeSize);
		float xdistForward = (gridSpotx)*xCubeSize + .5*xCubeSize - location[0];
		if(gridSpotx>0){
			firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v;
			if(gridSpoty+1<ySplit) firstTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].v;
			else firstTopSideAverage = 0;
		}else{
			firstBottomSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v;
			if(gridSpoty+1<ySplit) firstTopSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].v;
			else firstTopSideAverage = 0;
		}
		if((gridSpotz*zCubeSize) + .5*zCubeSize < location[2]){//over z half way point calculate for z+1
			float zdistBack = location[2] - ((gridSpotz*zCubeSize) + .5*zCubeSize);
			float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
			if(gridSpotz+1<zSplit){
				if(gridSpotx>0){
					secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].v;
					else secondTopSideAverage = 0;
				}else{
					secondBottomSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].v;
					else secondTopSideAverage = 0;
				}
			}
			fullBottomSideAverage = (zdistForward/zCubeSize)*firstBottomSideAverage + (zdistBack/zCubeSize)*secondBottomSideAverage;
			fullTopSideAverage = (zdistForward/zCubeSize)*firstTopSideAverage + (zdistBack/zCubeSize)*secondTopSideAverage;
		} else{//calculate for y-1
			float zdistBack = location[2] - ((gridSpotz-1)*zCubeSize + .5*zCubeSize);
			float zdistForward = (gridSpotz)*zCubeSize + .5*zCubeSize - location[2];
			if(gridSpotz>0){
				if(gridSpotx>0){
					secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].v;
					else secondTopSideAverage = 0;
				}else{
					secondBottomSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].v;
					if(gridSpoty+1<ySplit) secondTopSideAverage = (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].v;
					else secondTopSideAverage = 0;
				}
			}else{
				secondBottomSideAverage = 0;
				secondTopSideAverage = 0;
			}
			fullBottomSideAverage = (zdistBack/zCubeSize)*firstBottomSideAverage + (zdistForward/zCubeSize)*secondBottomSideAverage;
			fullTopSideAverage = (zdistBack/zCubeSize)*firstTopSideAverage + (zdistForward/zCubeSize)*secondTopSideAverage;
		}
		vfullAverage = (ydistBack/yCubeSize)*fullTopSideAverage + (ydistForward/yCubeSize)*fullBottomSideAverage;
	}
	return vec3(ufullAverage, vfullAverage, wfullAverage);
}
