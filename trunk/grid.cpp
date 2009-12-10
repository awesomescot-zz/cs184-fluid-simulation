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
	for(int i=0; i<xSplit; i++){
		for(int j=0; j<ySplit; j++){
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

	//W
	float zdistBack = location[2] - (gridSpotz*zCubeSize);
	float zdistForward = (gridSpotz+1)*zCubeSize - location[2];
	if( (gridSpotx*xCubeSize) + .5*xCubeSize < location[0]){//if the point is over half way through the cube in the x direction
		float xdistBack = location[0] - (gridSpotx*xCubeSize) + .5*xCubeSize;
		float xdistForward = (gridSpotx+1)*xCubeSize + .5*xCubeSize - location[0];
		float firstCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].w;
		float firstFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].w;
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - (gridSpoty*yCubeSize) + .5*yCubeSize;
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			float secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].w;
			float secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz+1].w;
			float fullCloseSideAverage = (ydistForward/yCubeSize)*firstCloseSideAverage + (ydistBack/yCubeSize)*secondCloseSideAverage;
			float fullFarSideAverage = (ydistForward/yCubeSize)*firstFarSideAverage + (ydistBack/yCubeSize)*secondFarSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - (gridSpoty-1)*yCubeSize + .5*yCubeSize;
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			float secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].w;
			float secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz+1].w;
			float fullCloseSideAverage = (ydistBack/yCubeSize)*firstCloseSideAverage + (ydistForward/yCubeSize)*secondCloseSideAverage;
			float fullFarSideAverage = (ydistBack/yCubeSize)*firstFarSideAverage + (ydistForward/yCubeSize)*secondFarSideAverage;
		}
		float wfullAverage = (zdistBack/zCubeSize)*fullFarSideAverage + (zdistForward/zCubeSize)*fullCloseSideAverage;
	}else{
		float xdistBack = location[0] - (gridSpotx-1)*xCubeSize + .5*xCubeSize;
		float xdistForward = (gridSpotx)*xCubeSize + .5*xCubeSize - location[0];
		float rightSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].w;
		float leftSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].w;
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - (gridSpoty*yCubeSize) + .5*yCubeSize;
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			float secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].w;
			float secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].w;
			float fullCloseSideAverage = (ydistForward/yCubeSize)*firstCloseSideAverage + (ydistBack/yCubeSize)*secondCloseSideAverage;
			float fullFarSideAverage = (ydistForward/yCubeSize)*firstFarSideAverage + (ydistBack/yCubeSize)*secondFarSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - (gridSpoty-1)*yCubeSize + .5*yCubeSize;
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			float secondCloseSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty-1][gridSpotz].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].w;
			float secondFarSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty-1][gridSpotz+1].w + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].w;
			float fullCloseSideAverage = (ydistBack/yCubeSize)*firstCloseSideAverage + (ydistForward/yCubeSize)*secondCloseSideAverage;
			float fullFarSideAverage = (ydistBack/yCubeSize)*firstFarSideAverage + (ydistForward/yCubeSize)*secondFarSideAverage;
		}
		float wfullAverage = (zdistBack/zCubeSize)*fullFarSideAverage + (zdistForward/zCubeSize)*fullCloseSideAverage;

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
		float zdistBack = location[2] - (gridSpotz*zCubeSize) + .5*zCubeSize;
		float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
		float firstLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u + (zdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].u;
		float firstRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].u;
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - (gridSpoty*yCubeSize) + .5*yCubeSize;
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			float secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].u;
			float secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz+1].u;
			float fullLeftSideAverage = (ydistForward/yCubeSize)*firstLeftSideAverage + (ydistBack/yCubeSize)*secondLeftSideAverage;
			float fullRightSideAverage = (ydistForward/yCubeSize)*firstRightSideAverage + (ydistBack/yCubeSize)*secondRightSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - (gridSpoty-1)*yCubeSize + .5*yCubeSize;
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			float secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz+1].u;
			float secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz+1].u;
			float fullLeftSideAverage = (ydistBack/yCubeSize)*firstLeftSideAverage + (ydistForward/yCubeSize)*secondLeftSideAverage;
			float fullRightSideAverage = (ydistBack/yCubeSize)*firstRightSideAverage + (ydistForward/yCubeSize)*secondRightSideAverage;
		}
		float ufullAverage = (xdistBack/xCubeSize)*fullRightSideAverage + (xdistForward/xCubeSize)*fullLeftSideAverage;
	}else{
		float zdistBack = location[2] - (gridSpotz-1)*zCubeSize + .5*zCubeSize;
		float zdistForward = (gridSpotz)*zCubeSize + .5*zCubeSize - location[2];
		float firstLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].u + (zdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].u;
		float firstRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].u;
		if((gridSpoty*yCubeSize) + .5*yCubeSize < location[1]){//over y half way point calculate for y+1
			float ydistBack = location[1] - (gridSpoty*yCubeSize) + .5*yCubeSize;
			float ydistForward = (gridSpoty+1)*yCubeSize + .5*yCubeSize - location[1];
			float secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].u;
			float secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].u;
			float fullLeftSideAverage = (ydistForward/yCubeSize)*firstLeftSideAverage + (ydistBack/yCubeSize)*secondLeftSideAverage;
			float fullRightSideAverage = (ydistForward/yCubeSize)*firstRightSideAverage + (ydistBack/yCubeSize)*secondRightSideAverage;
		} else{//calculate for y-1
			float ydistBack = location[1] - (gridSpoty-1)*yCubeSize + .5*yCubeSize;
			float ydistForward = (gridSpoty)*yCubeSize + .5*yCubeSize - location[1];
			float secondLeftSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx][gridSpoty-1][gridSpotz].u;
			float secondRightSideAverage = (zdistForward/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz-1].u + (zdistBack/zCubeSize)*cubeGrid[gridSpotx+1][gridSpoty-1][gridSpotz].u;
			float fullLeftSideAverage = (ydistBack/yCubeSize)*firstLeftSideAverage + (ydistForward/yCubeSize)*secondLeftSideAverage;
			float fullRightSideAverage = (ydistBack/yCubeSize)*firstRightSideAverage + (ydistForward/yCubeSize)*secondRightSideAverage;
		}
		float ufullAverage = (xdistBack/xCubeSize)*fullRightSideAverage + (xdistForward/xCubeSize)*fullLeftSideAverage;
	}

	//V
	float ydistBack = location[1] - (gridSpoty*yCubeSize);
	float ydistForward = (gridSpoty+1)*yCubeSize - location[1];
	if( (gridSpotx*xCubeSize) + .5*xCubeSize < location[0]){//if the point is over half way through the cube in the x direction
		float xdistBack = location[0] - (gridSpotx*xCubeSize) + .5*xCubeSize;
		float xdistForward = (gridSpotx+1)*xCubeSize + .5*xCubeSize - location[0];
		float firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz].v;
		float firstTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz].v;
		if((gridSpotz*zCubeSize) + .5*zCubeSize < location[2]){//over z half way point calculate for z+1
			float zdistBack = location[2] - (gridSpotz*zCubeSize) + .5*zCubeSize;
			float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
			float secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz+1].v;
			float secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz+1].v;
			float fullBottomSideAverage = (zdistForward/zCubeSize)*firstBottomSideAverage + (zdistBack/zCubeSize)*secondBottomSideAverage;
			float fullTopSideAverage = (zdistForward/zCubeSize)*firstTopSideAverage + (zdistBack/zCubeSize)*secondTopSideAverage;
		} else{//calculate for y-1
			float zdistBack = location[2] - (gridSpotz-1)*zCubeSize + .5*zCubeSize;
			float zdistForward = (gridSpotz)*zCubeSize + .5*zCubeSize - location[2];
			float secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty][gridSpotz-1].v;
			float secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx+1][gridSpoty+1][gridSpotz-1].v;
			float fullBottomSideAverage = (zdistBack/zCubeSize)*firstBottomSideAverage + (zdistForward/zCubeSize)*secondBottomSideAverage;
			float fullTopSideAverage = (zdistBack/zCubeSize)*firstTopSideAverage + (zdistForward/zCubeSize)*secondTopSideAverage;
		}
		float vfullAverage = (ydistBack/yCubeSize)*fullTopSideAverage + (ydistForward/yCubeSize)*fullBottomSideAverage;
	}else{
		float xdistBack = location[0] - (gridSpotx-1)*xCubeSize + .5*xCubeSize;
		float xdistForward = (gridSpotx)*xCubeSize + .5*xCubeSize - location[0];
		float firstBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz].v;
		float firstTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz].v;
		if((gridSpotz*zCubeSize) + .5*zCubeSize < location[2]){//over z half way point calculate for z+1
			float zdistBack = location[2] - (gridSpotz*zCubeSize) + .5*zCubeSize;
			float zdistForward = (gridSpotz+1)*zCubeSize + .5*zCubeSize - location[2];
			float secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz+1].v;
			float secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz+1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz+1].v;
			float fullBottomSideAverage = (zdistForward/zCubeSize)*firstBottomSideAverage + (zdistBack/zCubeSize)*secondBottomSideAverage;
			float fullTopSideAverage = (zdistForward/zCubeSize)*firstTopSideAverage + (zdistBack/zCubeSize)*secondTopSideAverage;
		} else{//calculate for y-1
			float zdistBack = location[2] - (gridSpotz-1)*zCubeSize + .5*zCubeSize;
			float zdistForward = (gridSpotz)*zCubeSize + .5*zCubeSize - location[2];
			float secondBottomSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty][gridSpotz-1].v;
			float secondTopSideAverage = (xdistForward/xCubeSize)*cubeGrid[gridSpotx-1][gridSpoty+1][gridSpotz-1].v + (xdistBack/xCubeSize)*cubeGrid[gridSpotx][gridSpoty+1][gridSpotz-1].v;
			float fullBottomSideAverage = (zdistBack/zCubeSize)*firstBottomSideAverage + (zdistForward/zCubeSize)*secondBottomSideAverage;
			float fullTopSideAverage = (zdistBack/zCubeSize)*firstTopSideAverage + (zdistForward/zCubeSize)*secondTopSideAverage;
		}
		float vfullAverage = (ydistBack/yCubeSize)*fullTopSideAverage + (ydistForward/yCubeSize)*fullBottomSideAverage;
	}
	return vec3(ufullAverage, vfullAverage, wfullAverage);
}
