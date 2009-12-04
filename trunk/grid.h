#ifndef _grid_h
#define _grid_h

using namespace std;

class grid {
public:
	vec3 location;
	grid(){};
	grid(vec3);
	ray getRay(vec3);
	void setCord(vec3);
};

#endif
