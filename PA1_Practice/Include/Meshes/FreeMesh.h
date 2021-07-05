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
		glEnableVertexAttribArray(1);
		glDrawArrays(primitiveType, 0, vertices.size());
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	void SendToGPU() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

		glBindVertexArray(0);
	}

	void AddArrayOfVertices(Vertex vertexArray[], int size) {
		for (int i = 0; i < size; i++)
			vertices.push_back(vertexArray[i]);
		std::cout << "# of vertices in free mesh: " << vertices.size() << std::endl;
	}
private:


};