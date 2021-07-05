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

