#include "Sphere.h"



SphereData Sphere::genSphere(unsigned int sectorCount, unsigned int stackCount, float radius) {
	float* vertices = new float[(stackCount + 1) * (sectorCount + 1) * 8]; // just verts n texes right now
	float PI = 3.14159;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	unsigned int v = 0;
	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

													// add (sectorCount+1) vertices per stack
													// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

													// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			*(vertices + v++) = x;
			*(vertices + v++) = y;
			*(vertices + v++) = z;

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			*(vertices + v++) = nx;
			*(vertices + v++) = ny;
			*(vertices + v++) = nz;

			//	normals.push_back(glm::vec3(nx, ny, nz));

			// vertex tex coord (s, t) range between [0, 1]
			*(vertices + v++) = (float)j / sectorCount;
			*(vertices + v++) = (float)i / stackCount;

		}
	}


	// generate CCW index list of sphere triangles
	GLuint* indices = new GLuint[(stackCount) * (sectorCount) * 6];
	int k1, k2;
	unsigned int c = 0;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				*(indices + c++) = k1;
				*(indices + c++) = k2;
				*(indices + c++) = (k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				*(indices + c++) = (k1 + 1);
				*(indices + c++) = (k2);
				*(indices + c++) = (k2 + 1);
			}
		}
	}

	return SphereData{ vertices, indices, v, c };

}
