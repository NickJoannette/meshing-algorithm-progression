#pragma once

#include <glm/glm.hpp>

#include <../Common.h>
#include <vector>
using namespace glm;

//given a grid cell, returns the set of triangles that approximates the region where val == 0.
std::vector<vec3> Polygonise(GRIDCELL& Grid, /*TriMeshFace* */ int Triangles, int& NewVertexCount);//, vec3* Vertices)