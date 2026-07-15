#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>

//TODO: refactor the camera class with a better system
class Camera {
public:

	//void LookAt() {
	//	glm::mat4 lookAt{
	//		glm::vec4(m_right, 0.0f),
	//		glm::vec4(m_up, 0.0f),
	//		glm::vec4(m_direction, 0.0f),
	//		glm::vec4(0.0f),
	//	};

	//	glm::mat4 trans{ 1.0f };
	//	trans = glm::translate(trans, -m_position);
	//	lookAt = lookAt * trans;

	//	std::cout << "right: "     << glm::to_string(m_right)     << std::endl;
	//	std::cout << "up: "		   << glm::to_string(m_up)        << std::endl;
	//	std::cout << "direction: " << glm::to_string(m_direction) << std::endl;
	//	std::cout << "position: "  << glm::to_string(m_position)  << std::endl;
	//	std::cout << "trans: "     << glm::to_string(trans)       << std::endl;
	//	std::cout << "LookAt: "    << glm::to_string(lookAt)      << std::endl;

	//	//lookAt
	//	//(RX, RY, RZ, 0)   (1, 0, 0, -PX)
	//	//(UX, UY, UZ, 0) * (0, 1, 0, -PY)
	//	//(DX, DY, DZ, 0)   (0, 0, 1, -PZ)
	//	//(0 , 0 , 0 , 1)   (0, 0, 0, 1  )
	//}

	inline glm::vec3 getPosition() const { return m_position; }

	inline void setLimit(glm::vec3 limit) { m_limit = limit; }

	glm::mat4 move(glm::vec3 velocity) {
		m_position += velocity;
		m_target = velocity + m_target;
		m_direction = glm::normalize(m_position - m_target);

		glm::mat4 lookAt{ 1.0f };

		lookAt = glm::lookAt(m_position, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
		return lookAt;
	}

	glm::mat4 setPosition(glm::vec3 position) { 
		m_position = position; 
		glm::vec3 new_target = position + m_target;
		m_direction = glm::normalize(m_position - new_target);

		glm::mat4 lookAt{ 1.0f };

		lookAt = glm::lookAt(m_position, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
		return lookAt;

	}
	glm::mat4 setTarget(glm::vec3 target) {
		m_target = target;
		m_direction = glm::normalize(m_position - m_target);

		glm::mat4 lookAt{ 1.0f };

		lookAt = glm::lookAt(m_position, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
		return lookAt;
	}
private:
	glm::vec3 m_position;
	glm::vec3 m_target;

	glm::vec3 m_direction; //= target - position

	glm::vec3 m_limit;
};