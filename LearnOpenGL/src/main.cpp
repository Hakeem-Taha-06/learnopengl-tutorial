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
	std::string SHADER_SOURCE_PATH = "src/shaders/";
#else
	#ifdef RELEASE
		std::string SHADER_SOURCE_PATH = "shaders/";
	#endif
#endif

//event callbacks
void OnFrameBufferResize(GLFWwindow* window, int width, int height);
void OnWindowClose(GLFWwindow* window);

//core
void setGLFWEventCallbacks(GLFWwindow* window);
void processInput(GLFWwindow* window);

//shader file handling
std::string readFileContents(const char* filePath);
unsigned int createShaderProgram(const char* vPath, const char* fPath);

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
	
	//create the shader program that uses the vertex and fragment shaders in the shaders folder
	unsigned int shaderProgram1 = createShaderProgram((SHADER_SOURCE_PATH + "VertexShader.vert").c_str(),
													  (SHADER_SOURCE_PATH + "FragmentShader.frag").c_str());
	unsigned int shaderProgram2 = createShaderProgram((SHADER_SOURCE_PATH + "VertexShader.vert").c_str(),
													  (SHADER_SOURCE_PATH + "FragmentShader2.frag").c_str());
													  //kinda fragile, but at least the debug and release builds work

	//rendering example
	//vertex data that will be copied into the vertex buffer
	float recVertices[] = {
		-0.5f, -0.5f,  0.0f, //bottom left
		 0.5f, -0.5f,  0.0f, //bottom right
		-0.5f,  0.5f,  0.0f, //top left
		 0.5f,  0.5f,  0.0f, //top right
	};

	//draw order indices, will be stored in an Element buffer object
	unsigned int recIndices[] = {
		0, 1, 2, //first triangle
		1, 2, 3, //second triangle
	};

	float triangleVertices[] = {
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.0f,  0.75f, 0.0f,
	};

	unsigned int triangleIndices[] = {
		0, 1, 2,
	};

	//---------------------------------------------------------------------------------
	//more compact, but just assumes everything is structured the exact same way, which probably won't happen.
	//you COULD however store another struct that would contain the attribute pointer stuff for each VAO + the drawing method for the rendering loop
	//std::vector<std::vector<float>> vertices = {all the vertex data vectors...}
	//std::vector<std::vector<unsigned int>> indices = {all the index data vectors...}
	//unsigned int VAOs[16], VBOs[16], EBOs[16];
	/*for (int i = 0; i < vertices.size(); ++i) {
		glGenBuffers(1, &VBOs[i]);
		glGenBuffers(1, &EBOs[i]);
		glGenVertexArrays(1, &VAOs[i]);

		glBindVertexArray(VAOs[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(float), vertices[i].data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(float), indices[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}*/

	//------------------------------------------------------------------
	//create a vertex array object, a vertex buffer object and an element buffer object
	//vertex array: will contain the attributes associated with a VBO as well as calls to glEnableVertexAttribArray/glDisableVertexAttribArray etc...
	//vertex buffer: will contain the vertex data that will be sent to the GPU
	//element buffer: will contain the indices for drawing order
	//------------------ rectangle data ------------------
	unsigned int VAO_r, VBO_r, EBO_r;
	glGenBuffers(1, &VBO_r);
	glGenBuffers(1, &EBO_r);
	glGenVertexArrays(1, &VAO_r);

	//now the properties for the bound buffers below should be "recorded" in the VAO
	glBindVertexArray(VAO_r);

	//bind the VBO as an array buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO_r);
	//copy the vertex data into the VBO (now accessed as the array buffer since we bound it earlier)
	glBufferData(GL_ARRAY_BUFFER, sizeof(recVertices), recVertices, GL_STATIC_DRAW);

	//similarly copy the indices to the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_r);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(recIndices), recIndices, GL_STATIC_DRAW);
	

	//set the vertex atrribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind VAO FIRST, then unbind the EBO
	//if you unbind EBO first then you basically undefined the EBO for this VAO
	//NOTE: unbinding is not really necessary if you bind the VAO you want to use everytime, which is something you would always do anyways ig
	/*glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	std::cout << "VAO_r: " << VAO_r << " VBO_r: " << VBO_r << " EBO_r: " << EBO_r << std::endl;
	std::cout << "Is VAO_r valid ->" << glIsVertexArray(VAO_r) <<std::endl;
	std::cout << "--------------------------" << std::endl;

	//------------------ triangle data ------------------
	unsigned int VAO_t, VBO_t, EBO_t;
	glGenBuffers(1, &VBO_t);
	glGenBuffers(1, &EBO_t);
	glGenVertexArrays(1, &VAO_t);

	glBindVertexArray(VAO_t);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_t);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_t);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

	std::cout << "VAO_t: " << VAO_t << " VBO_t: " << VBO_t << " EBO_t: " << EBO_t << std::endl;
	std::cout << "Is VAO_t valid -> " << glIsVertexArray(VAO_t) << std::endl;
	std::cout << "--------------------------" << std::endl;

	//we need to define how the gpu should interpret this data
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
	
	//for wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
		
		//rectangle
		glUseProgram(shaderProgram1);
		glBindVertexArray(VAO_r); // we bind the VAO that we want to use for drawing
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//triangle
		glUseProgram(shaderProgram2);
		glBindVertexArray(VAO_t); 
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "GL error: " << err << std::endl;
		}

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

unsigned int createShaderProgram(const char* vPath, const char* fPath) {
	//-----------------------------------------
	//shader creation and compilation (can be abstracted into a helper function)
	//-----------------------------------------
	//----------vertex shader----------
	//create a vertex shader object
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//set the shader's source code
	std::string vertexShaderSource = readFileContents(vPath);
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
		exit(EXIT_FAILURE);
	}

	//----------fragment shader----------
	unsigned int fragShader;
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string fragShaderSource = readFileContents(fPath);
	const char* fsrc = fragShaderSource.c_str();
	glShaderSource(fragShader, 1, &fsrc, nullptr);

	glCompileShader(fragShader);

	int fCompileStatus;
	char fInfoLog[MAX_INFO_SIZE];
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fCompileStatus);
	if (fCompileStatus == GL_FALSE) {
		glGetShaderInfoLog(fragShader, MAX_INFO_SIZE, NULL, fInfoLog);
		std::cout << "shader compilation error: " << fInfoLog << std::endl;
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}

	//activate the program
	glUseProgram(shaderProgram);

	//delete the shaders as they are no longer needed
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}

//reads a shader source file and returns a string containing the code
std::string readFileContents(const char* filePath) {
	std::ifstream file;
	std::stringstream ss;

	file.exceptions(std::fstream::badbit);
	try {

		file.open(filePath, std::ios_base::in);
		if (!file.is_open()) {
			std::cerr << "error opening file: "<< filePath << std::endl;
			exit(EXIT_FAILURE);
		}

		ss << file.rdbuf();

		file.close();
	}
	catch (std::fstream::failure e) {
		std::cerr << "File \""<< filePath << "\" could not open: " 
			<< "(" << e.what() << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ss.str();
}