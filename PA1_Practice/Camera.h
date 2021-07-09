#ifndef CAMERA_H
#define CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLWindow.h"
#include "PhysicsCommon.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ROTATE_RIGHT,
	ROTATE_LEFT
};

// Default orientations
const float YAW = -90.0f;
const float PITCH = -glm::degrees(atanf(10.0f / 30.0f));
const float SPEED = 4.0f;

const float SENSITIVITY = 1.0;
const float ZOOM = 120.0f;


class Camera
{
public:

	void applyGravity() {

		Acceleration = -WorldUp * GRAVITATIONAL_ACCELERATION;

	}

	void physicsUpdate(float dt, bool collisionBelow) {
		Velocity += Acceleration;
		Position += Velocity;
		
	}

	OpenGLWindow * mainWindow;

	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Velocity;
	glm::vec3 Acceleration;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Perspective matrix
	glm::mat4 projection;

	// Tilt/pan Angles
	float Yaw;
	float Pitch;

	// Camera Speeds and Sensitivities
	float MovementSpeed;
	float MouseSensitivity = 0.2f;
	float ZoomSensitivity = 0.05f;
	float Zoom;

	// Constructor with vectors
	Camera(OpenGLWindow * mainwindow, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : 
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = PITCH;
		mainWindow = mainwindow;
		updateView();
		UpdateProjection();
	}

	// Returns the camera's view matrix using glm::lookAt
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 * GetProjectionMatrix() { return &projection; }

	void UpdateProjection() {
		projection = glm::perspective(glm::radians(Zoom), mainWindow->getAspectRatio(), 0.01f, 1000.0f);
	}

	// Per-frame keyboard input process function
	void ProcessKeyboard(Camera_Movement direction, float dt)
	{

		float velocity = MAX_PLAYER_SPEED * dt;
		glm::vec3 cameraZAxis = glm::cross(-Right, WorldUp);
		if (direction == FORWARD)
			Position += cameraZAxis * velocity;
		else if (direction == BACKWARD)
			Position -= cameraZAxis * velocity;
		else if (direction == LEFT)
			Position -= Right * velocity;
		else if (direction == RIGHT)
			Position += Right * velocity;
		else if (direction == DOWN)
			Position += glm::vec3(0, -1.0, 0) * velocity;
		else if (direction == UP)
			Position += glm::vec3(0, 1.0, 0) *velocity;

	}

	void ProcessTilt(float tiltQty)
	{
		
		Pitch += tiltQty;
		if (Pitch > 89.9f) {
			Pitch -= tiltQty;
		}
		else if (Pitch < -89.9f) {
			Pitch -= tiltQty;
		}
		updateView();
	}

	void ProcessPan(float xoffset)
	{
		Yaw -= xoffset;
		updateView();
	}
	
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{

		Zoom -= yoffset;
		UpdateProjection();
	}

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateView()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
private:
	
};
#endif
