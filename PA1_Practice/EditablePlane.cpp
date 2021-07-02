#include "EditablePlane.h"

EditablePlane::Vertex3 * EditablePlane::getNearestVertex(glm::vec3 point) {
	float nearestDistance = std::numeric_limits<float>::max();
	Vertex3* nearestVertex = nullptr;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < length; j++) {
			float distance = glm::distance(point, glmVec3(vertices[i][j]));
			if (distance < nearestDistance) {
				nearestDistance = distance;
				nearestVertex = &vertices[i][j];
			}
		}
	}
	return nearestVertex;
}

bool EditablePlane::withinBounds(glm::vec3 point) {
	return abs(point.x) <= 100 && abs(point.z) <= 100;
}

bool EditablePlane::intersectRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
	float distanceFromRayToIntersection = -1.0f;
	glm::intersectRayPlane(rayOrigin, rayDirection, center, glm::vec3(0, 1, 0), distanceFromRayToIntersection);
	glm::vec3 pointOfIntersection = rayOrigin + distanceFromRayToIntersection * rayDirection;
	return (distanceFromRayToIntersection > 0 && withinBounds(pointOfIntersection));
}

EditablePlane::Vertex3 * EditablePlane::getNearestVertexToIntersectingRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
	float distanceFromRayToIntersection = -1.0f;
	glm::intersectRayPlane(rayOrigin, rayDirection, center, glm::vec3(0, 1, 0), distanceFromRayToIntersection);
	glm::vec3 pointOfIntersection = rayOrigin + distanceFromRayToIntersection * rayDirection;
	if (distanceFromRayToIntersection > 0 && withinBounds(pointOfIntersection)) {
		return getNearestVertex(pointOfIntersection);
	}

	else return nullptr;
}


EditablePlane::EditablePlane(unsigned int w, unsigned int l)
{
	// We need at least 2 vertices on either side for a square.
	//if (w == 1 && l == 1) w = l = 2;
	//w++; l++; // Increment these by so that the correct # of squares is drawn (require w+l, l+1 for the algo)
	
	// 100x100 squares hardcoded
	w = width; l = length;

	int c = 0, c2 = 0;
	for (int j = 0; j < l; j++) {
		float z = (float)j - l/2.0f+0.5f;
		for (int i = 0; i < w; i++) {
			float x = (float)i  - l/2.0f+0.5f;
			float y = 0;
			vertices[j][i].x = x;
			vertices[j][i].y = y;
			vertices[j][i].z = z;
			vertices[j][i].state = 0.0f;
			//(vertices + c)->x = x;
			//(vertices + c)->y = y; 
			//(vertices + c++)->z = z;
		}
	}

	

	
	calculateIndicesAndNormals();

	// Create a new vertex array object to store the Grid's info.
	glGenVertexArrays(1, &vao);

	// Create a buffer for the vertex data and one for the indices.
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
		
	sendVerticesToGPU();

	// Clear allocated memory now that it's been sent to the GPU. We don't need it on the CPU anymore.
	//delete[8 * w * l] vertices;
	//delete[6 * (w - 1) * (l - 1)] indices;

}

void EditablePlane::sendVerticesToGPU() {
	// Bind the vertex array to signal to OpenGL that we are referring to it.
	glBindVertexArray(vao);

	// Bind the VBO and pass in the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (width) * (length) * sizeof(Vertex3), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	// Inform OpenGL of the way the vertex data will be passed into the shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)(3*sizeof(float)));
	// Bind the EBO and pass in the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * (width - 1) * (length - 1) * sizeof(GLuint), indices, GL_STATIC_DRAW);
	// Not sure if these two calls below are necessary but I hear they're good practice
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void EditablePlane::calculateIndicesAndNormals() {
	unsigned int w = width, l = length;
	indices = new GLuint[6 * (w - 1) * (l - 1)];	
	int c = 0;
	int ct = 0;
	for (int i = 0; i < (w-1) * (l-1); i++, c++) {
		if ((c + 1) % w == 0) ++c;
		// Indices for one face
		*(indices + ct++) = c + l;
		*(indices + ct++) = c + 1;
		*(indices + ct++) = c;

		// Indices for the 'diagonal' face on the grid
		*(indices + ct++) = c + 1;
		*(indices + ct++) = c + w;
		*(indices + ct++) = c + w + 1;
	}
}

void EditablePlane::Draw(GLint primitiveType)
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawElements(primitiveType, 6 * (width - 1) * (length - 1), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}
