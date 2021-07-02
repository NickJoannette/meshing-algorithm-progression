#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <../Shader.h>
#include <../Common.h>
#include <chrono>
#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>

class FreeMesh {

public:
	struct Vertex {
		float x, y, z;
	};

	unsigned int VBO;
	unsigned int VAO;

	unsigned int numVertices;
	std::vector<Vertex> vertices;
	
	FreeMesh() {
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
	}

	void Draw(GLint primitiveType) {
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(primitiveType, 0, numVertices);
		glDisableVertexAttribArray(0);
	}

	void SendToGPU() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, &vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glBindVertexArray(0);
	}

private:


};