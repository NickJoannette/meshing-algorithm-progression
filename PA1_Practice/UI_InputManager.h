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
	bool wPressed = false;
	bool shiftPressed = false;
	bool qPressed = false;
	bool zPressed = false;
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

	
	void processInput()
	{



		if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_PRESS) {
			wPressed = true;
		}
		else wPressed = false;

		
		if (glfwGetKey(mWind, GLFW_KEY_Q) == GLFW_PRESS) {
			qPressed = true;
		}
		else qPressed = false;

		
		if (glfwGetKey(mWind, GLFW_KEY_Z) == GLFW_PRESS) {
			zPressed = true;
		}
		else zPressed = false;


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
		if (glfwGetKey(mWind, GLFW_KEY_SPACE) == GLFW_PRESS) {

			spacePressed = true;
		}
		//else spacePressed = false;


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

