#pragma once

struct Bounds {
	float minX, maxX, minY, maxY, minZ, maxZ;
};

struct Voxel {
	glm::vec3 start;
	float density;
	int type;
	bool culled = false;
	Bounds bounds;
	float width = 1, length = 1, height = 1;
};



struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float tx, ty;
	int type;
};


struct TRIANGLE {
	glm::vec3 p[3]; // 3 points of the triangle
};

struct FACE {
	TRIANGLE tri;
	glm::vec3 normal;

};


/* Indexing convention:

			 Vertices:                    Edges:

		  4  ______________ 5           ______________
		   /|            /|           /|     4      /|
		  / |         6 / |       7  / |8        5 / |
	  7  /_____________/  |        /______________/  | 9
		|   |         |   |        |   |   6     |   |
		| 0 |_________|___| 1      |   |_________|10_|
		|  /          |  /      11 | 3/     0    |  /
		| /           | /          | /           | / 1
	  3 |/____________|/ 2         |/____________|/
										  2
 */


struct GRIDCELL {
	glm::vec3 p[8];		//position of each corner of the grid in world space

	// For every one of these corners, there are 7 vertex neighbors
	// The vertex is a different index relative to each of them 



	float val[8];	//value of the function at this grid corner

	FACE faces[5];

	unsigned int numFaces = 0;


};


