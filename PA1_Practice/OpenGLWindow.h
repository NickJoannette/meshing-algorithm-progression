#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#undef main

class OpenGLWindow
{
public:

	OpenGLWindow(float WIDTH, float HEIGHT);
	~OpenGLWindow();

	void swapBuffers();
	void clearColor(float r, float g, float b, float a);
	inline float getWidth() { return WIDTH; }
	inline float getHeight() { return HEIGHT; }
	inline float getAspectRatio() { return ASPECT_RATIO; }

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void processKeyInput(GLFWwindow* window);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	inline GLFWwindow * glWindow() { return window; }

private:

	GLuint gProgramID = 0;

	float WIDTH, HEIGHT, ASPECT_RATIO;
	GLFWwindow* window;
};

