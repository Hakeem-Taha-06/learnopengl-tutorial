#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> //glm::to_string

#include <iostream>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	ASCEND,
	DESCEND
};

//default attributes
const glm::vec3 POS{ 0.0f, 0.0f, 0.0f };
const glm::vec3 UP { 0.0f, 1.0f, 0.0f };
const float SPEED = 3.0f;
const float PITCH = 0.0f;
const float YAW = -90.0f;
const float SENSITIVITY = 0.1f;
const float	ZOOM = 45.0f;


class Camera {
public:
	Camera(glm::vec3 pos = POS, glm::vec3 up = UP,float pitch = PITCH, float yaw = YAW, float speed = SPEED)
		:Position(pos), WorldUp(up), Pitch(pitch), Yaw(yaw), Speed(speed), 
		 Sensitivity(SENSITIVITY), Zoom(ZOOM), Front(glm::vec3(0.0f, 0.0f, -1.0f))
	{
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

	void processKeyInput(CameraMovement direction, float deltaTime) {
		if (direction == FORWARD) 
			Position += Speed * Front * deltaTime;
		if (direction == BACKWARD) 
			Position -= Speed * Front * deltaTime;
		if (direction == LEFT) 
			Position -= Speed * Right * deltaTime;
		if (direction == RIGHT) 
			Position += Speed * Right * deltaTime;
		if (direction == ASCEND)
			Position += Speed * WorldUp * deltaTime;
		if (direction == DESCEND)
			Position -= Speed * WorldUp * deltaTime;
	}

	void processMouseMovement(float xoffset, float yoffset) {
		xoffset *= Sensitivity;
		yoffset *= Sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;
		//limit the pitch to prevent flipping
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		//to make the debug output more readable, limit the ranger from -180 to 180
		if (Yaw >= 180)
			Yaw -= 360;
		if (Yaw <= -180)
			Yaw += 360;

		//std::cout << "pitch: " << Pitch << std::endl;
		//std::cout << "yaw: " << Yaw << std::endl;

		updateCameraVectors();
	}

	void processMouseScroll(float yoffset) {
		Zoom -= (float)yoffset;
		if (Zoom >= 45.0) {
			Zoom = 45.0f;
		}
		if (Zoom <= 1.0) {
			Zoom = 1.0f;
		}
	}

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	float Speed;
	float Pitch;
	float Yaw;
	float Sensitivity;
	float Zoom;

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};