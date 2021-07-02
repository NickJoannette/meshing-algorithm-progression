#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "shader.h"
#include <chrono>
#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>

class Line
{

public:

#pragma region line buffer objects
	unsigned int lineVBO;
	unsigned int lineVAO;

	glm::mat4 lineTransform = glm::mat4(1.0f);
	glm::mat4 lineModel = glm::mat4(1.0f);

	Line(float x1, float y1, float z1, float x2, float y2, float z2) {
		float vertices[] =
		{
			x1, y1, z1, // First point
			x2, y2, z2  // Second point
		};
		glGenBuffers(1, &lineVBO);
		glGenVertexArrays(1, &lineVAO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);
	}

	glm::mat4* getTransformMatrix() { return &lineTransform; }
	glm::mat4* getModelMatrix() { return &lineModel; }
	void setTransform(glm::mat4 newTransform) { lineTransform = newTransform; }
	void resetTransform() { lineTransform = glm::mat4(1.0f); }

	void Draw(GLint primitiveType) {
		glBindVertexArray(lineVAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableVertexAttribArray(0);
	}
#pragma endregion line

};