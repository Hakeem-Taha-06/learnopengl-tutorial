#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Shader.h"

#include <iostream>
#include <cmath>

typedef struct {
	float r, g, b, a;
}Color;

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
	
	//create the shader program that uses the vertex and fragment shaders in the shaders folder
	Shader shader1((SHADER_SOURCE_PATH + "VertexShader.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader.frag").c_str());
	Shader shader2((SHADER_SOURCE_PATH + "VertexShader2.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader2.frag").c_str());
	Shader shader3((SHADER_SOURCE_PATH + "VertexShader3.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader3.frag").c_str());
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

	float topTriVertices[] = {
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.0f,  0.75f, 0.0f,
	};

	//unnecessary for a simple triangle, just for demostration purposes
	unsigned int topTriIndices[] = {
		0, 1, 2,
	};

	float botTriVertices[] = {
		//position             //color
		-0.5f, -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.0f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
	};

	unsigned int botTriIndices[] = {
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

	std::cout << "VAO_r: " << VAO_r << " VBO_r: " << VBO_r << " EBO_r: " << EBO_r << std::endl;
	std::cout << "Is VAO_r valid ->" << glIsVertexArray(VAO_r) <<std::endl;
	std::cout << "--------------------------" << std::endl;

	//------------------ top triangle data ------------------
	unsigned int VAO_tt, VBO_tt, EBO_tt;
	glGenBuffers(1, &VBO_tt);
	glGenBuffers(1, &EBO_tt);
	glGenVertexArrays(1, &VAO_tt);

	glBindVertexArray(VAO_tt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_tt);
	glBufferData(GL_ARRAY_BUFFER, sizeof(topTriVertices), topTriVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_tt);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(topTriIndices), topTriIndices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	std::cout << "VAO_tt: " << VAO_tt << " VBO_tt: " << VBO_tt << " EBO_tt: " << EBO_tt << std::endl;
	std::cout << "Is VAO_tt valid -> " << glIsVertexArray(VAO_tt) << std::endl;
	std::cout << "--------------------------" << std::endl;
	
	//------------------ bottom triangle data ------------------
	unsigned int VAO_bt, VBO_bt, EBO_bt;
	glGenBuffers(1, &VBO_bt);
	glGenBuffers(1, &EBO_bt);
	glGenVertexArrays(1, &VAO_bt);

	glBindVertexArray(VAO_bt);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO_bt);
	glBufferData(GL_ARRAY_BUFFER, sizeof(botTriVertices), botTriVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_bt);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(botTriIndices), botTriIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	std::cout << "VAO_bt: " << VAO_bt << " VBO_bt: " << VBO_bt << " EBO_tt: " << EBO_bt << std::endl;
	std::cout << "Is VAO_bt valid -> " << glIsVertexArray(VAO_bt) << std::endl;
	std::cout << "--------------------------" << std::endl;

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

		glBindVertexArray(VAO_r); // we bind the VAO that we want to use for drawing
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//top triangle
		shader2.use();
		glBindVertexArray(VAO_tt); 
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//bottom triangle
		shader3.use();
		shader3.setUniformf("hOffset", 0.5);
		glBindVertexArray(VAO_bt);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

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
}
