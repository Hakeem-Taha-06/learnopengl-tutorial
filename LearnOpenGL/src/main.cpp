#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

typedef struct {
	float r, g, b, a;
}Color;

#define MAX_INFO_SIZE 512

//window specs
const int WIDTH = 800;
const int HEIGHT = 600;

//colors
const Color BG_BLUE{ 0.2f, 0.3f, 0.5f, 1.0f };

//shader source paths
#ifdef DEBUG
	const char* vertexSourcePath = "src/shaders/VertexShader.vert";
	const char* fragmentSourcePath = "src/shaders/FragmentShader.frag";
#else
	#ifdef RELEASE
		const char* vertexSourcePath = "shaders/vShaderSource.vert";
		const char* fragmentSourcePath = "shaders/fShaderSource.frag";
	#endif
#endif




//event callbacks
void OnFrameBufferResize(GLFWwindow* window, int width, int height);
void OnWindowClose(GLFWwindow* window);

//core
void setGLFWEventCallbacks(GLFWwindow* window);
void processInput(GLFWwindow* window);

//shader file handling
std::string readShaderFile(const char* shaderSourcePath);

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

	//init GLAD by giving it the platform specific function that loads openGL's function pointers
	//glfw provides a function that does that vvvvvvv
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	//set openGL rendering window size
	//starts from the lower left (not top left) corner of the window and covers the full width and height
	glViewport(0, 0, WIDTH, HEIGHT);

	//hookup event callbacks to the appropriate functions that we defined
	setGLFWEventCallbacks(window);

	//-----------------------------------------
	//shader creation and compilation (can be abstracted into a helper function)
	//-----------------------------------------
	//----------vertex shader----------
	//create a vertex shader object
	unsigned int vertexShader; 
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//set the shader's source code
	std::string vertexShaderSource = readShaderFile(vertexSourcePath);
	const char* vsrc = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &vsrc, nullptr);

	//compile the shader and check for compilation errors
	glCompileShader(vertexShader);

	int vCompileStatus;
	char vInfoLog[MAX_INFO_SIZE];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vCompileStatus);
	if (vCompileStatus == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, MAX_INFO_SIZE, NULL, vInfoLog);
		std::cout << "shader compilation error: " << vInfoLog << std::endl;
		return -1;
	}

	//----------fragment shader----------
	unsigned int fragShader;
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string fragShaderSource = readShaderFile(fragmentSourcePath);
	const char* fsrc = fragShaderSource.c_str();
	glShaderSource(fragShader, 1, &fsrc, nullptr);

	glCompileShader(fragShader);

	int fCompileStatus;
	char fInfoLog[MAX_INFO_SIZE];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &fCompileStatus);
	if (fCompileStatus == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, MAX_INFO_SIZE, NULL, fInfoLog);
		std::cout << "shader compilation error: " << fInfoLog << std::endl;
		return -1;
	}

	//-----------------------------------------
	//shader program creation and linking
	//-----------------------------------------
	//create the shader program object
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	//attach shaders and link them and check for linking errors
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	int linkStatus;
	char pInfoLog[MAX_INFO_SIZE];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		glGetProgramInfoLog(shaderProgram, MAX_INFO_SIZE, nullptr, pInfoLog);
		std::cerr << "program linking error: " << pInfoLog << std::endl;
		return -1;
	}

	//activate the program
	glUseProgram(shaderProgram);

	//delete the shaders as they are no longer needed
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	//triangle rendering example
	//vertex data that will be copied into the vertex buffer
	float vertices[] = {
		-0.5f,  -0.5f, 0.0f,
		 0.5f,  -0.5f, 0.0f,
		 0.0f,   0.5f, 0.0f,
	};

	//create a vertex buffer object and a vertex array object.
	//vertex buffer: will contain the vertex data that will be sent to the GPU
	//vertex array: will contain the attributes associated with a VBO as well as calls to glEnableVertexAttribArray/glDisableVertexAttribArray
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	//bind the VBO as an array buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//copy the vertex data into the VBO (now accessed as the array buffer since we bound it earlier)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//set the vertex atrribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//atp both the vertex data and shader processing instructions have been sent to the gpu
	//so we need to define how the gpu should interpret this data
	//we want to use three vertices, each one has 3 coordinates (x, y, z) with no space between each two vertices
	//      V1             V2             V3
	//[x,   y,   z], [x,   y,   z], [x,   y,   z]
	//    --12--         --12--         --12--
	// 4    4    4    4    4    4    4    4    4
	//each vertex is 12 bytes, consisting of 3 floats, 4 bytes each

	//parameters in order (for the glVertexAttribPointer call):
	//0, since we specified location = 0 in the vertex shader
	//size is 3, for a vec3 in GLSL
	//type is GL_FLOAT for a vec* in GLSL
	//no normalization
	//stride is 3 * the size of a float (stride = 12)
	//no offset in the vertex data
	

	//main loop
	while (!glfwWindowShouldClose(window)) {

		//input
		processInput(window);

		//rendering
		glClearColor(BG_BLUE.r, 
					 BG_BLUE.g, 
					 BG_BLUE.b, 
					 BG_BLUE.a);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(vertexShader);
		glBindVertexArray(VAO); //in case there are other VAOs, we bind the one we want to use
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
}

//reads a shader source file and returns a string containing the code
std::string readShaderFile(const char* shaderSourcePath) {
	std::ifstream shaderSourceFile;
	std::stringstream shaderStream;

	shaderSourceFile.exceptions(std::fstream::badbit);
	try {

		shaderSourceFile.open(shaderSourcePath, std::ios_base::in);
		if (!shaderSourceFile.is_open()) {
			std::cerr << "error opening file: "<< shaderSourcePath << std::endl;
			exit(EXIT_FAILURE);
		}

		shaderStream << shaderSourceFile.rdbuf();

		shaderSourceFile.close();
	}
	catch (std::fstream::failure e) {
		std::cerr << "Shader file \""<< shaderSourcePath << "\" could not open: " 
			<< "(" << e.what() << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	return shaderStream.str();
}