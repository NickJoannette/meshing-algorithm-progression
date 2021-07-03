#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <../Shader.h>
#include <chrono>
#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <../Common.h>

class Cube
{

public:

#pragma region cube buffer objects
	unsigned int cubeVBO;
	unsigned int cubeVAO;
	unsigned int cubeEBO;

	glm::mat4 cubeTransform = glm::mat4(1.0f);
	glm::mat4 cubeModel = glm::mat4(1.0f);


	Cube() {
		cubeModel = glm::mat4(1);
		glGenBuffers(1, &cubeVBO);
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 36, cubeVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}

	glm::mat4 * getTransformMatrix() { return &cubeTransform; }
	glm::mat4 * getModelMatrix() { return &cubeModel; }
	void setTransform(glm::mat4 newTransform) { cubeTransform = newTransform; }
	void resetTransform() { cubeTransform = glm::mat4(1.0f); }




	void Draw(GLint primitiveType) {
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		//glEnableVertexAttribArray(2);
		glDrawArrays(primitiveType, 0, 36);
		glDisableVertexAttribArray(2);
		//glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

	}

	// set up the vertices for the cube
// ------------------------------------------------------------------
	Vertex cubeVertices[36] = {

		Vertex{0.0f,0.0f,0.0f, -1.0f,-1.0f,-1.0f, 0.0f, 0.0f,0},
		Vertex{0.0f,0.0f, 1.0f, -1.0f,-1.0f,1.0f, 0.0f, 1.0f,0 },
		Vertex{0.0f, 1.0f, 1.0f, -1.0f,1.0f,1.0f, 1.0f, 1.0f, 0},
		Vertex{0.0f,0.0f,0.0f,-1.0f,-1.0f,-1.0f, 0.0f, 0.0f, 0},
		Vertex{0.0f, 1.0f, 1.0f,-1.0f,1.0f,1.0f, 1.0f, 1.0f, 0},
		Vertex{0.0f, 1.0f,0.0f,-1.0f,1.0f,-1.0f, 1.0f, 0.0f, 0},
		Vertex{ 1.0f,1.0f,0.0f, 1.0f,1.0f,-1.0f, 1.0f, 1.0f, 0},
		Vertex{0.0f,0.0f,0.0f, -1.0f,-1.0f,-1.0f, 0.0f, 0.0f, 0},
		Vertex{0.0f, 1.0f,0.0f, -1.0f,1.0f,-1.0f, 0.0f, 1.0f, 0},
		Vertex{1.0f, 1.0f,0.0f,  1.0f,1.0f,-1.0f,1.0f, 1.0f, 0},
		Vertex{1.0f,0.0f,0.0f,  1.0f,-1.0f,-1.0f,1.0f, 0.0f, 0},
		Vertex{0.0f,0.0f,0.0f, -1.0f,-1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{1.0f,0.0f, 1.0f,  1.0f,-1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{0.0f,0.0f,0.0f, -1.0f,-1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{1.0f,0.0f,0.0f,  1.0f,-1.0f,-1.0f,1.0f, 0.0f, 0},
		Vertex{1.0f,0.0f, 1.0f,  1.0f,-1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{0.0f,0.0f, 1.0f, -1.0f,-1.0f,1.0f,0.0f, 1.0f, 0},
		Vertex{0.0f,0.0f,0.0f, -1.0f,-1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{0.0f, 1.0f, 1.0f,  -1.0f,1.0f,1.0f,0.0f, 1.0f, 0},
		Vertex{0.0f,0.0f, 1.0f,  -1.0f,-1.0f,1.0f,0.0f, 0.0f, 0},
		Vertex{1.0f,0.0f, 1.0f,   1.0f,-1.0f,1.0f,1.0f, 0.0f, 0},
		Vertex{1.0f, 1.0f, 1.0f,  1.0f,1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{0.0f, 1.0f, 1.0f, -1.0f,1.0f,1.0f,0.0f, 1.0f, 0},
		Vertex{1.0f,0.0f, 1.0f,  1.0f,-1.0f,1.0f,1.0f, 0.0f, 0},
		Vertex{1.0f, 1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{1.0f,0.0f,0.0f, 1.0f,-1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{1.0f, 1.0f,0.0f, 1.0f,1.0f,-1.0f,1.0f, 0.0f, 0},
		Vertex{1.0f,0.0f,0.0f, 1.0f,-1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{1.0f, 1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{1.0f,0.0f, 1.0f, 1.0f,-1.0f,1.0f,0.0f, 1.0f, 0},
		Vertex{1.0f, 1.0f, 1.0f,  1.0f,1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{1.0f, 1.0f,0.0f,  1.0f,1.0f,-1.0f,1.0f, 0.0f, 0},
		Vertex{0.0f, 1.0f,0.0f, -1.0f,1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{1.0f, 1.0f, 1.0f,  1.0f,1.0f,1.0f,1.0f, 1.0f, 0},
		Vertex{0.0f, 1.0f,0.0f, -1.0f,1.0f,-1.0f,0.0f, 0.0f, 0},
		Vertex{0.0f, 1.0f, 1.0f, -1.0f,1.0f,1.0f,0.0f, 1.0f,0}
	};


private:


};
