#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "../Shader.h"
#include <chrono>
#include <GL/glew.h>
#include "Icosphere.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>

struct SphereData { float* vertices; GLuint* indices; unsigned int numVertices; GLuint numIndices; };


class Sphere
{

public:

	glm::mat4 cubeTransform = glm::mat4(1.0f);
	glm::mat4 cubeModel = glm::mat4(1.0f);

	Sphere(float rad) {
		// Use Song Ho Ahn's icosphere class for the vertex/normal/tex data
		Icosphere icosphere(rad, 2.0, true);
		radius = icosphere.getRadius();
		icosphere.getVertices(); icosphere.getNormals();
		vertices = icosphere.getInterleavedVertices();
		indices = icosphere.getIndices();
		numVertices = icosphere.getInterleavedVertexCount();
		numIndices = icosphere.getIndexCount();
		allocateMeshOnGPU();
	}

	glm::mat4* getTransformMatrix() { return &cubeTransform; }
	glm::mat4* getModelMatrix() { return &cubeModel; }
	void setTransform(glm::mat4 newTransform) { cubeTransform = newTransform; }
	void resetTransform() { cubeTransform = glm::mat4(1.0f); }
	float getRadius() { return radius; }

	void Draw(GLint primitiveType) {

		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawElements(primitiveType, numIndices, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}
	void Draw(GLint primitiveType, unsigned int textureID1) {

		glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glDrawElements(primitiveType, numIndices, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);

		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}


	~Sphere() { };

private:

	unsigned int VAO, VBO, EBO;
	unsigned int numVertices;
	GLuint numIndices;
	float* vertices;
	GLuint* indices;
	float radius;
	Icosphere* icosphere;

	virtual void allocateMeshOnGPU()
	{

		glDisable(GL_CULL_FACE);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Buffer the vertex data
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 8, vertices, GL_STATIC_DRAW);

		// Position, then normal, then texture.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);



		glBindVertexArray(0);

	}


	SphereData genSphere(unsigned int sectorCount, unsigned int stackCount, float radius);


};
