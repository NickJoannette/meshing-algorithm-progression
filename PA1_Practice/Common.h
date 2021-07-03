#pragma once



struct Voxel {
	vec3 start;
	float density;
	int type;
	bool culled = false;
};



struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float tx, ty;
	int type;
};

