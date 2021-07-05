#pragma once

#include <glm/glm.hpp>

#include <../Common.h>
#include <vector>
using namespace glm;

struct GRIDCELL {
	vec3 p[8];		//position of each corner of the grid in world space
	float val[8];	//value of the function at this grid corner
};

//given a grid cell, returns the set of triangles that approximates the region where val == 0.
std::vector<vec3> Polygonise(GRIDCELL& Grid, /*TriMeshFace* */ int Triangles, int& NewVertexCount);//, vec3* Vertices)