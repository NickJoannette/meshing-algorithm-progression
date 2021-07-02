#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>
#include <map>
#include "shader.h"
#include <chrono>
#include <GL/glew.h>

class EditablePlane
{

public:

	struct Vertex3 {
	public:
		float x, y, z;
		float state;
	};

	EditablePlane() {  };
	EditablePlane(unsigned int w, unsigned int l);

	
	void sendVerticesToGPU();
	bool withinBounds(glm::vec3 point);
	bool intersectRay(glm::vec3 rayOrigin, glm::vec3 rayDirection);

	Vertex3* getNearestVertex(glm::vec3 point);
	Vertex3 * getNearestVertexToIntersectingRay(glm::vec3 rayOrigin, glm::vec3 rayDirection);

	inline glm::vec3 glmVec3(Vertex3 v) { return glm::vec3(v.x, v.y, v.z); }
		

	void Draw(GLint primitiveType);

	glm::mat4* getTransformMatrix() { return &transform; }
	void setTransform(glm::mat4 newTransform) { transform = newTransform; }

	void calculateIndicesAndNormals();

	unsigned int ebo;
	unsigned int vbo;
	unsigned int vao;

	GLuint* indices;
	Vertex3 vertices[101][101];

	unsigned int width=101, length=101;
	glm::mat4 transform = glm::mat4(1.0f);
	glm::vec3 center = glm::vec3(0.0f);

private:

};
