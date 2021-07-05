#pragma once
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include "OpenGLWindow.h"
#include "Camera.h"
#include "PhysicsCommon.h"

using namespace glm;

class UI_InputManager
{


public:

	// VARIABLES FOR ASSIGNMENT 1 FUNCTIONALITY

	// Keep track of mouse position for zoom/pan/tilt
	float mouseLastX = 0.0f, mouseLastY = 0.0f;
	bool rightMouseDown = false;
	bool leftMouseDown = false;
	bool spacePressed = false;
	bool shiftPressed = false;
	glm::vec3 rayDirection;

	// For keeping track of which primitive type is used to render
	GLint primitiveType = GL_TRIANGLES;

	// SCREEN DIMENSIONS KEPT AS CONSTANT FOR NOW
	const float SCR_WIDTH = 1920.0, SCR_HEIGHT = 1080.0;


	// Keep track of which model is currently selected by the F1...F5 keys


	bool capsLock = false;
	bool resetWorldOrientation = false;

	UI_InputManager(GLFWwindow * mWind, Camera * camera) {
		this->projection = camera->GetProjectionMatrix();
		this->mWind = mWind;
		this->camera = camera;

		// Register GLFW callbacks
		registerCallbacks();
	};

	
	void processInput(float dt)
	{

		if (glfwGetKey(mWind, GLFW_KEY_SPACE) == GLFW_PRESS) {

			spacePressed = true;
		}
		else spacePressed = false;
		
		if (glfwGetKey(mWind, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

			shiftPressed = true;
		}
		else shiftPressed = false;


		if (glfwGetKey(mWind, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, dt);
		if (glfwGetKey(mWind, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, dt);

		if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, dt);
		if (glfwGetKey(mWind, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, dt);

		if (glfwGetKey(mWind, GLFW_KEY_Q) == GLFW_PRESS)
			camera->ProcessKeyboard(UP, dt);
		if (glfwGetKey(mWind, GLFW_KEY_Z) == GLFW_PRESS)
			camera->ProcessKeyboard(DOWN, dt);



		if (glfwGetKey(mWind, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera->MovementSpeed = 6.0f;
		else camera->MovementSpeed = 4.0f;

		if (glfwGetKey(mWind, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(mWind, true);

		if (glfwGetKey(mWind, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			camera->MovementSpeed -= 0.0025f;

		if (glfwGetKey(mWind, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
			camera->MovementSpeed += 0.0025f;
		if (glfwGetKey(mWind, GLFW_KEY_0) == GLFW_PRESS)

		{
			camera->Pitch -= 1.0f;
			camera->updateView();
		}

	


	};


private:

	Camera * camera;
	GLFWwindow * mWind;
	glm::mat4 * projection;
	bool firstMouse = true;

#pragma region 	// Callback functions

	void registerCallbacks() {
		glfwSetWindowUserPointer(mWind, reinterpret_cast<void*>(this));
		glfwSetKeyCallback(mWind, keyCallBackPort);
		glfwSetFramebufferSizeCallback(mWind, framebufferSizeCallBackPort);
		glfwSetCursorPosCallback(mWind, mouseCallBackPort);
	};

	static void keyCallBackPort(GLFWwindow* window, int key, int scancode, int action, int mods) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			key_callback(window, key, scancode, action, mods);
	};

	static void framebufferSizeCallBackPort(GLFWwindow* window, int width, int height) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			framebuffer_size_callback(window, width, height);
	};

	static void mouseCallBackPort(GLFWwindow* window, double xpos, double ypos) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			mouse_callback(window, xpos, ypos);
	};

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// So we can resize the window dynamically
		glViewport(0, 0, width, height);
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
	
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			glm::vec3 f = camera->Front;
			glm::vec3 p = camera->Position;
			std::cout << "Camera is Facing: " << f.x << "," << f.y << "," << f.z << std::endl;
			std::cout << "Camera's Position is: " << p.x << "," << p.y << "," << p.z << std::endl;
			std::cout << "Camera's FOV angle is: " << camera->Zoom << std::endl;
		}

		if (glfwGetKey(mWind, GLFW_KEY_T) == GLFW_PRESS)
			primitiveType = GL_TRIANGLES;

		if (glfwGetKey(mWind, GLFW_KEY_P) == GLFW_PRESS)
			primitiveType = GL_POINTS;

		if (glfwGetKey(mWind, GLFW_KEY_L) == GLFW_PRESS)
			primitiveType = GL_LINES;






		// Pressing 'HOME' resets the camera position and pitch to the original values; focused at the origin.
		if (glfwGetKey(mWind, GLFW_KEY_HOME) == GLFW_PRESS) {
			camera->Position = glm::vec3(0.0f, 50.0, 0.0);
			camera->Pitch = -glm::degrees(atanf(10.0f / 30.0f));
			camera->Yaw = -90.0f;
			camera->Zoom = 87.5f;
			camera->updateView();
			camera->UpdateProjection();
			resetWorldOrientation = true;
		}                                                                                                                                                                                                                       


		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			GLint currentPolygonMode[2];
			glGetIntegerv(GL_POLYGON_MODE, currentPolygonMode);
			if (currentPolygonMode[0] == GL_FILL)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	};

	void mouse_callback(GLFWwindow* window, float xpos, float ypos)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			leftMouseDown = true;

		}
		else leftMouseDown = false;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { 
			float movementInY = mouseLastY - ypos; // how much the mouse was just dragged
			camera->ProcessTilt(movementInY * camera->MouseSensitivity);

		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			float movementInX = mouseLastX - xpos; // how much the mouse was just dragged
			camera->ProcessPan(movementInX * camera->MouseSensitivity);

		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
			float movementInY = mouseLastY - ypos; // how much the mouse was just dragged
			camera->ProcessMouseScroll(movementInY * camera->ZoomSensitivity);
		}

		mouseLastX = xpos;
		mouseLastY = ypos;






		glm::vec3 ray_wor;

		// Logic for getting mouse click ray in world coordinates

		glm::vec3 camPos = camera->Position;
		glm::mat4 transf = glm::inverse(*camera->GetProjectionMatrix());
		float x = (2.0f * (float)rayMouseX) / 1920.0f - 1.0f;
		float y = 1.0f - (2.0f * (float)rayMouseY) / 1080.0f;
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0);
		glm::vec4 ray_eye = glm::inverse(*camera->GetProjectionMatrix()) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, -ray_eye.y, -1.0, 0.0);
		ray_wor = (glm::inverse(camera->GetViewMatrix()) * ray_eye);
		rayDirection = glm::normalize(ray_wor);
		
		rayMouseX = xpos;
		rayMouseY = 1080.0f - ( abs(ypos));
	
		mouseLastX = xpos;
		mouseLastY = ypos;
	};

	float rayMouseX = 0;
	float rayMouseY = 0;


























#pragma endregion
};

