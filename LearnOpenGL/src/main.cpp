#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "Shader.h"
#include "Shape.h"
#include "Texture.h"
#include "Camera.h"

typedef struct {
	float r, g, b, a;
}Color;

//window specs
const int WIDTH = 800;
const int HEIGHT = 800;

//colors
const Color BG_BLUE{ 0.2f, 0.3f, 0.5f, 1.0f };
const Color BORDER{ 0.2f, 0.2f, 0.2f, 1.0f };

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
void onCursorMove(GLFWwindow* window, double xpos, double ypos);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

//core
GLFWwindow* Init();
void setGLFWEventCallbacks(GLFWwindow* window);
void processInput(GLFWwindow* window, float deltaTime);

float texInterp = 0.0f;
float FOV = 45.0f;

//camera 
Camera camera{glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f) ,-30.0f, -135.0f, 3.0f};

//light
glm::vec3 lightPos{ 0.5f, 0.8f, -0.5f };

//cursor properties
double mouseLastX = WIDTH / 2, mouseLastY = HEIGHT / 2;
bool firstMouse = true;

//delta time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
	
	GLFWwindow* window = Init();

	//create the shader program that uses the vertex and fragment shaders in the shaders folder
	Shader cubeShader((SHADER_SOURCE_PATH + "TextureVertexShader.vert").c_str(),
				      (SHADER_SOURCE_PATH + "TextureFragmentShader.frag").c_str());
	Shader lightShader((SHADER_SOURCE_PATH + "LightVertexShader.vert").c_str(),
		               (SHADER_SOURCE_PATH + "LightFragmentShader.frag").c_str());

	std::vector<float> cubeVertices{
		//positions         //color             //normals           //texture coordinates
		//back
		-0.5f,-0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,
		 0.5f,-0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f,
		 0.5f, 0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
		-0.5f, 0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f,

		//front
		-0.5f,-0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		 0.5f,-0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
		 0.5f, 0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,

		//left
		-0.5f, 0.5f, 0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, 0.5f,-0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		-0.5f,-0.5f,-0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-0.5f,-0.5f, 0.5f,  1.0f, 0.5f, 0.31f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		//right
		 0.5f, 0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		 0.5f, 0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		 0.5f,-0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		 0.5f,-0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		//bottom
		-0.5f,-0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f,
		 0.5f,-0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f,
		 0.5f,-0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f,
		-0.5f,-0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,

		//top
		-0.5f, 0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		 0.5f, 0.5f,-0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		 0.5f, 0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,  1.0f, 0.5f, 0.31f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	};

	std::vector<unsigned int> cubeIndices = {
		//back
		0, 1, 2,
		2, 3, 0,

		//front
		4, 5, 6,
		6, 7, 4,

		//left
		8, 9, 10,
		10, 11, 8,

		//right
		12, 13, 14,
		14, 15, 12,

		//bottom
		16, 17, 18,
		18, 19, 16,

		//top
		20, 21, 22,
		22, 23, 20,
	};

	//---------------------------------------------------------------------------------

	//------------------ cube data ------------------
	Shape cube(cubeVertices, cubeIndices);
	cube.create(PosColNormTex3d, GL_STATIC_DRAW);

	Shape light(cubeVertices, cubeIndices);
	//reuse the cube's buffers since they are already sent to the gpu
	light.setVBO(cube.getVBO());
	light.setEBO(cube.getEBO());
	light.create(PosColNormTex3d, GL_STATIC_DRAW);

	//texture stuff
	Texture texture1((ASSETS_PATH + "textures/cool_cat.png").c_str());
	Texture texture2((ASSETS_PATH + "textures/urara_ballin.png").c_str());

	cubeShader.use();
	cubeShader.setUniformi("texture1", 0);
	cubeShader.setUniformi("texture2", 1);

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

		cubeShader.use();
		//coordinate transformation pipeline (MVP pipeline)
		//model matrix: local -> world
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		cubeShader.setUniformMat4("model", glm::value_ptr(model));

		//view matrix: world -> view
		glm::mat4 view(1.0f);
		view = camera.getViewMatrix();
		cubeShader.setUniformMat4("view", glm::value_ptr(view));

		//projection matrix: view -> clip
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		cubeShader.setUniformMat4("projection", glm::value_ptr(projection));

		texture1.setUnit(0);
		texture2.setUnit(1);
		
		cubeShader.setUniformf("lightColor", 1.0f, 1.0f, 1.0f);
		cubeShader.setUniformf("ambientStrength", 0.2f);
		cubeShader.setUniformf("specularStrength", 0.5f);
		cubeShader.setUniformVec3("lightPos", lightPos);
		cubeShader.setUniformVec3("cameraPos", camera.Position);
		cubeShader.setUniformf("shine", 32);

		cube.draw(GL_TRIANGLES);

		lightShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setUniformMat4("model", glm::value_ptr(model));

		view = glm::mat4(1.0f);
		view = camera.getViewMatrix();
		lightShader.setUniformMat4("view", glm::value_ptr(view));

		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		lightShader.setUniformMat4("projection", glm::value_ptr(projection));

		light.draw(GL_TRIANGLES);

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

GLFWwindow* Init() {
	//init glfw
	if (!glfwInit()) {
		std::cerr << "glfw not initialized";
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}
	//set the window as the current context
	//context is the set of properties that define opengl's state
	glfwMakeContextCurrent(window);

	//Vsync on
	glfwSwapInterval(1);

	//hides the cursor and sets it to the middle of the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//init GLAD by giving it the platform specific function that loads openGL's function pointers
	//glfw provides a function that does that vvvvvvv
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD";
		exit(EXIT_FAILURE);
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

	return window;
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

	//camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyInput(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyInput(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyInput(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyInput(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyInput(ASCEND, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.processKeyInput(DESCEND, deltaTime);
}

void OnFrameBufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void OnWindowClose(GLFWwindow* window) {
	std::cout << "Closing window..." << std::endl;
}

void onCursorMove(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse) {
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouse = false;
	}

	double xoffset = (xpos - mouseLastX);
	double yoffset = (mouseLastY - ypos); //flip y
	mouseLastX = xpos;
	mouseLastY = ypos;

	camera.processMouseMovement((float)xoffset, (float)yoffset);
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processMouseScroll((float)yoffset);
}


void setGLFWEventCallbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, OnFrameBufferResize);
	glfwSetWindowCloseCallback(window, OnWindowClose);
	glfwSetCursorPosCallback(window, onCursorMove);
	glfwSetScrollCallback(window, onMouseScroll);
}
