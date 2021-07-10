#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "../Shader.h"
#include <chrono>
#include <GL/glew.h>


#define GLM_ENABLE_EXPERIMENTAL
using namespace glm;
#include <glm/gtc/matrix_transform.hpp>

class Line
{

public:

#pragma region line buffer objects
	unsigned int lineVBO;
	unsigned int lineVAO;

	glm::mat4 lineTransform = glm::mat4(1.0f);
	glm::mat4 lineModel = glm::mat4(1.0f);
	vec3 v1, v2;

	Line(vec3 v1, vec3 v2) {



		this->v1 = v1;
		this->v2 = v2;
		float vertices[] = {
			v1.x, v1.y, v1.z, // First point
			v2.x, v2.y, v2.z  // Second point
		};
		glGenBuffers(1, &lineVBO);
		glGenVertexArrays(1, &lineVAO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);




	}

	vec3 vector() {
		return v2 - v1;
	}

	float length() {
		return distance(v1, v2);
	}

	glm::mat4* getTransformMatrix() { return &lineTransform; }
	glm::mat4* getModelMatrix() { return &lineModel; }
	void setTransform(glm::mat4 newTransform) { lineTransform = newTransform; }
	void resetTransform() { lineTransform = glm::mat4(1.0f); }

	void Draw() {
		glBindVertexArray(lineVAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableVertexAttribArray(0);
	}

	~Line() {
		glDeleteBuffers(1, &lineVBO);
		glDeleteVertexArrays(1, &lineVAO);
	}
#pragma endregion line

};