#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <print>

#include "Shader.h"
#include "Shape.h"
#include "Texture.h"
#include "Camera.h"

typedef struct {
	float r, g, b, a;
}Color;

//window specs
const int WIDTH = 600;
const int HEIGHT = 600;

//colors
const Color BG_BLUE{ 0.2f, 0.3f, 0.5f, 1.0f };
const Color BORDER{ 0.2f, 0.2f, 0.2f, 1.0f };

//TODO: remove old camera stuff
auto camera = glm::vec3(10.0f, 50.0f, 10.0f);

constexpr auto CAMERA_LIMIT = glm::vec3(100.0f, 100.0f, 100.0f);
constexpr auto CAMERA_SPEED = 10.0f;

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
void processInput(GLFWwindow* window, float deltaTime);

float texInterp = 0.0f;
float FOV = 45.0f;

glm::vec3 cameraPos  {0.0f, 0.0f, 3.0f};
glm::vec3 cameraFront{0.0f, 0.0f, -1.0f};
glm::vec3 cameraUp   {0.0f, 1.0f, 0.0f};
float cameraSpeed = 3.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
	std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

	//set openGL rendering window size
	//starts from the lower left (not top left) corner of the window and covers the full width and height
	glViewport(0, 0, WIDTH, HEIGHT);

	//enables depth testing via z-buffer
	glEnable(GL_DEPTH_TEST);
	
	GLint depthBits = 0;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depthBits);
	std::cout << "Depth buffer bits: " << depthBits << std::endl;

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
	std::vector<float> recVertices{
		-1.0f, -1.0f,  0.0f, //bottom left
		 1.0f, -1.0f,  0.0f, //bottom right
		-1.0f,  1.0f,  0.0f, //top left
		 1.0f,  1.0f,  0.0f, //top right
	};

	//draw order indices, will be stored in an Element buffer object
	std::vector<unsigned int> recIndices{
		0, 1, 2, //first triangle
		1, 2, 3, //second triangle
	};

	std::vector<float> topTriVertices{
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.0f,  0.75f, 0.0f,
	};

	//unnecessary for a simple triangle, just for demostration purposes
	std::vector<unsigned int> topTriIndices{
		0, 1, 2,
	};

	std::vector<float> botTriVertices{
		//position             //color
		-0.5f, -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.0f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
	};

	std::vector<unsigned int> botTriIndices{
		0, 1, 2,
	};

	std::vector<float> sqVertices{
		 //position           //color             //texture coordinates
		 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 1.0f,    //top
		 0.5f,  0.0f, 0.0f,	  0.0f, 1.0f, 0.0f,   1.0f, 0.5f,    //right
		 0.0f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.5f, 0.0f,    //bottom
		-0.5f,  0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.5f,    //left
	};

	std::vector<unsigned int> sqIndices{
		0, 1, 2,
		2, 3, 0,
	};

	std::vector<float> cubeVertices{
		//positions         //color            //texture coordinates
		//bottom
		-0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		 0.5f,-0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		 0.5f, 0.5f,-0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-0.5f, 0.5f,-0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

		//top
		-0.5f,-0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		 0.5f,-0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		 0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

		//left
		-0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f,-0.5f,-0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
		-0.5f,-0.5f, 0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,

		//right
		 0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		 0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		 0.5f,-0.5f,-0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
		 0.5f,-0.5f, 0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,


		//back
		-0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		 0.5f,-0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		 0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
		-0.5f,-0.5f, 0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,

		//front
		-0.5f, 0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		 0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		 0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	};

	std::vector<unsigned int> cubeIndices = {

		//top
		4, 5, 6,
		6, 7, 4,

		//front
		16, 17, 18,
		18, 19, 16,

		
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

	//------------------ cube data ------------------
	Shape cube(cubeVertices, cubeIndices);
	cube.create(VertexDataShape::PosColTex3d, GL_STATIC_DRAW);

	//texture stuff
	Texture texture1((ASSETS_PATH + "textures/cool_cat.png").c_str());
	Texture texture2((ASSETS_PATH + "textures/urara_ballin.png").c_str());

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

		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input
		processInput(window, deltaTime);

		//rendering
		glClearColor(BG_BLUE.r, 
					 BG_BLUE.g, 
					 BG_BLUE.b, 
					 BG_BLUE.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//coordinate transformation pipeline (MVP pipeline)
		//model matrix: local -> world
		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		//view matrix: world -> view
		glm::mat4 view(1.0f);
		view = glm::lookAt(cameraPos, cameraFront+ cameraPos, cameraUp);

		//projection matrix: view -> clip
		glm::mat4 projection = glm::perspective(glm::radians(FOV), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		texture1.setUnit(0);
		texture2.setUnit(1);
		
		shader4.use();

		shader4.setUniformf("texInterp", texInterp);

		shader4.setUniformMat4("model", glm::value_ptr(model));
		shader4.setUniformMat4("view", glm::value_ptr(view));
		shader4.setUniformMat4("projection", glm::value_ptr(projection));

		trans = glm::mat4(1.0f);
		//trans = glm::rotate(trans, -(float)glfwGetTime() * 2, glm::vec3(0.0f, 0.0f, 1.0f));

		shader4.setUniformMat4("transform", glm::value_ptr(trans));
		cube.draw(GL_TRIANGLES);
		for (int i = 0; i < 50; ++i) {
			for (int j = 0; j < 50; ++j) {
				model = glm::mat4(1.0f);
				model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(i * 1.0f, j*1.0f, j * 1.0f));
				shader4.setUniformMat4("model", glm::value_ptr(model));
				cube.draw(GL_TRIANGLES);
			}
		}
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

void processInput(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}

	//there's probably a better way to do this
	if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS) {
		texInterp += 0.01f;
		if (texInterp >= 1.0) {
			texInterp = 1.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
		texInterp -= 0.01f;
		if (texInterp <= 0.0) {
			texInterp = 0.0f;
		}
	}

	//TODO: remove old camera controls
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera.z += 0.01f * CAMERA_SPEED;
		if (camera.z >= CAMERA_LIMIT.z) {
			camera.z = CAMERA_LIMIT.z;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera.z -= 0.01f * CAMERA_SPEED;
		if (camera.z <= -CAMERA_LIMIT.z) {
			camera.z = -CAMERA_LIMIT.z;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera.x += 0.01f * CAMERA_SPEED;
		if (camera.x >= CAMERA_LIMIT.x) {
			camera.x = CAMERA_LIMIT.x;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera.x -= 0.01f * CAMERA_SPEED;
		if (camera.x <= -CAMERA_LIMIT.x) {
			camera.x = -CAMERA_LIMIT.x;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.y += 0.01f * CAMERA_SPEED;
		if (camera.y >= CAMERA_LIMIT.y) {
			camera.y = CAMERA_LIMIT.y;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.y -= 0.01f * CAMERA_SPEED;
		if (camera.y <= -CAMERA_LIMIT.y) {
			camera.y = -CAMERA_LIMIT.y;
		}
	}

	//camera controls
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront,cameraUp)) * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp)) * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		FOV += 0.1f;
		if (FOV >= 90.0) {
			FOV = 90.0f;
		}
		std::cout << "FOV: " << FOV << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		FOV -= 0.1f;
		if (FOV <= 0.0) {
			FOV = 0.0f;
		}
		std::cout << "FOV: "<< FOV << std::endl;
	}
}
