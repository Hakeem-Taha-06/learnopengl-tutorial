#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "stb/stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "Shape.h"
#include "Texture.h"

#include <iostream>
#include <cmath>
#include <print>

typedef struct {
	float r, g, b, a;
}Color;

typedef struct {
	float x, y, z;
}Camera;

//window specs
const int WIDTH = 600;
const int HEIGHT = 600;

//colors
const Color BG_BLUE{ 0.2f, 0.3f, 0.5f, 1.0f };
const Color BORDER{ 0.2f, 0.2f, 0.2f, 1.0f };

Camera camera{0.0f, 0.0f, 3.0f};

//shader source paths
#ifdef DEBUG
	std::string SHADER_SOURCE_PATH = "src/shaders/";
	std::string ASSETS_PATH = "src/assets/";
#else
	#ifdef RELEASE
		std::string SHADER_SOURCE_PATH = "shaders/";
		std::string ASSETS_PATH = "assets/";
	#endif
#endif

//event callbacks
void OnFrameBufferResize(GLFWwindow* window, int width, int height);
void OnWindowClose(GLFWwindow* window);

//core
void setGLFWEventCallbacks(GLFWwindow* window);
void processInput(GLFWwindow* window);

float a = 0.0f;

int main() {

	//init glfw
	if (!glfwInit()) {
		std::cerr << "glfw not initialized";
		return -1;
	}

	//set opengl version to 3 and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window object
	//an object is a struct that contains a bunch of properties for opengl
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello, Window!", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create window";

		glfwTerminate();
		return -1;
	}
	//set the window as the current context
	//context is the set of properties that define opengl's state
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//init GLAD by giving it the platform specific function that loads openGL's function pointers
	//glfw provides a function that does that vvvvvvv
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

	//set openGL rendering window size
	//starts from the lower left (not top left) corner of the window and covers the full width and height
	glViewport(0, 0, WIDTH, HEIGHT);

	//hookup event callbacks to the appropriate functions that we defined
	setGLFWEventCallbacks(window);

	//opengl expects y = 0 to be the bottom line, while images have it at the top
	stbi_set_flip_vertically_on_load(true);
	
	//create the shader program that uses the vertex and fragment shaders in the shaders folder
	Shader shader1((SHADER_SOURCE_PATH + "VertexShader.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader.frag").c_str());
	Shader shader2((SHADER_SOURCE_PATH + "VertexShader2.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader2.frag").c_str());
	Shader shader3((SHADER_SOURCE_PATH + "VertexShader3.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader3.frag").c_str());
	Shader shader4((SHADER_SOURCE_PATH + "VertexShader4.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader4.frag").c_str());
		//kinda fragile, but at least the debug and release builds work

	//rendering example
	//vertex data that will be copied into the vertex buffer
	std::vector<float> recVertices = {
		-1.0f, -1.0f,  0.0f, //bottom left
		 1.0f, -1.0f,  0.0f, //bottom right
		-1.0f,  1.0f,  0.0f, //top left
		 1.0f,  1.0f,  0.0f, //top right
	};

	//draw order indices, will be stored in an Element buffer object
	std::vector<unsigned int> recIndices = {
		0, 1, 2, //first triangle
		1, 2, 3, //second triangle
	};

	std::vector<float> topTriVertices = {
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.0f,  0.75f, 0.0f,
	};

	//unnecessary for a simple triangle, just for demostration purposes
	std::vector<unsigned int> topTriIndices = {
		0, 1, 2,
	};

	std::vector<float> botTriVertices = {
		//position             //color
		-0.5f, -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.0f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
	};

	std::vector<unsigned int> botTriIndices = {
		0, 1, 2,
	};

	std::vector<float> sqVertices = {
		 //position           //color             //texture coordinates
		 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 1.0f,    //top
		 0.5f,  0.0f, 0.0f,	  0.0f, 1.0f, 0.0f,   1.0f, 0.5f,    //right
		 0.0f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.5f, 0.0f,    //bottom
		-0.5f,  0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.5f,    //left
	};

	std::vector<unsigned int> sqIndices = {
		0, 1, 2,
		2, 3, 0,
	};

	//---------------------------------------------------------------------------------

	Shape rectangle(recVertices, recIndices);
	rectangle.create(VertexDataShape::Pos3d, GL_STATIC_DRAW);

	//------------------ top triangle data ------------------
	Shape topTriangle(topTriVertices, topTriIndices);
	topTriangle.create(VertexDataShape::Pos3d, GL_STATIC_DRAW);
	
	//------------------ bottom triangle data ------------------
	Shape bottomTriangle(botTriVertices, botTriIndices);
	bottomTriangle.create(VertexDataShape::PosCol3d, GL_STATIC_DRAW);

	//------------------ middle square data ------------------
	Shape square(sqVertices, sqIndices);
	square.create(VertexDataShape::PosColTex3d, GL_STATIC_DRAW);

	//texture stuff
	Texture texture1((ASSETS_PATH + "textures/urara_ballin.png").c_str());
	Texture texture2((ASSETS_PATH + "textures/cool_cat.png").c_str());

	shader4.use();
	shader4.setUniformi("texture1", 0);
	shader4.setUniformi("texture2", 1);


	//------------------------- transformations -------------------------
	glm::vec4 vector(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans(1.0f);

	trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	vector = trans * vector;

	std::print("vector: ({}, {}, {})\n", vector.x, vector.y, vector.z);

	//for wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//----------------------- main loop -----------------------
	while (!glfwWindowShouldClose(window)) {

		//input
		processInput(window);

		//rendering
		glClearColor(BG_BLUE.r, 
					 BG_BLUE.g, 
					 BG_BLUE.b, 
					 BG_BLUE.a);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//rectangle
		shader1.use();
		float time = (float)glfwGetTime();
		Color customColor = {std::sin(time)/2.0f + 0.5f, 0.0f, std::cos(time)/2.0f + 0.5f, 1.0f};
		shader1.setUniformf("customColor", customColor.r,
										   customColor.g,
										   customColor.b,
										   customColor.a);
		rectangle.draw(GL_TRIANGLES);
		////top triangle
		//shader2.use();
		//shader2.setUniformi("isInverted", 0); //bool uniform can take in an int
		//topTriangle.draw(GL_TRIANGLES);
		//
		////bottom triangle
		//shader3.use();
		//shader3.setUniformf("hOffset", 0.0f);
		//bottomTriangle.draw(GL_TRIANGLES);

		//coordinate transformation pipeline (MVP pipeline)
		//model matrix: local -> world
		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		//view matrix: world -> view
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(-camera.x, -camera.y, -camera.z));

		//projection matrix: view -> clip
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);

		//middle square
		texture1.setUnit(0);
		texture2.setUnit(1);

		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
		trans = glm::rotate(trans, (float)glfwGetTime()*2, glm::vec3(0.0f, 0.0f, 1.0f));
		
		shader4.use();
		shader4.setUniformMat4("model", glm::value_ptr(model));
		shader4.setUniformMat4("view", glm::value_ptr(view));
		shader4.setUniformMat4("projection", glm::value_ptr(projection));

		shader4.setUniformMat4("transform", glm::value_ptr(trans));
		shader4.setUniformf("a", a);
		square.draw(GL_TRIANGLES);

		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, -(float)glfwGetTime() * 2, glm::vec3(0.0f, 0.0f, 1.0f));

		shader4.setUniformMat4("transform", glm::value_ptr(trans));
		square.draw(GL_TRIANGLES);

		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, (float)glfwGetTime() * 2, glm::vec3(0.0f, 0.0f, 1.0f));

		shader4.setUniformMat4("transform", glm::value_ptr(trans));
		square.draw(GL_TRIANGLES);

		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
		trans = glm::rotate(trans, -(float)glfwGetTime() * 2, glm::vec3(0.0f, 0.0f, 1.0f));

		shader4.setUniformMat4("transform", glm::value_ptr(trans));
		square.draw(GL_TRIANGLES);

		/*GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "GL error: " << err << std::endl;
		}*/

		//check and poll events and swap frame buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//frees all allocated memory by GLFW
	glfwTerminate();
	return 0;
}

void OnFrameBufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void OnWindowClose(GLFWwindow* window) {
	std::cout << "Closing window..." << std::endl;
}

void setGLFWEventCallbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, OnFrameBufferResize);
	glfwSetWindowCloseCallback(window, OnWindowClose);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}

	//there's probably a better way to do this
	if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS) {
		a += 0.01f;
		if (a >= 1.0) {
			a = 1.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
		a -= 0.01f;
		if (a <= 0.0) {
			a = 0.0f;
		}
	}

	//camera controls
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera.z += 0.01f;
		if (camera.z >= 3.0) {
			camera.z = 3.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera.z -= 0.01f;
		if (camera.z <= -3.0) {
			camera.z = -3.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera.x += 0.01f;
		if (camera.x >= 3.0) {
			camera.x = 3.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera.x -= 0.01f;
		if (camera.x <= -3.0) {
			camera.x = -3.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.y += 0.01f;
		if (camera.y >= 3.0) {
			camera.y = 3.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.y -= 0.01f;
		if (camera.y <= -3.0) {
			camera.y = -3.0f;
		}
	}
}
