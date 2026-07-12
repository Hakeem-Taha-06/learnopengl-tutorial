#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Shader.h"
#include "Shape.h"

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
const Color BORDER{ 0.2f, 0.2f, 0.2f, 1.0f };

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
	Shader shader4((SHADER_SOURCE_PATH + "VertexShader4.vert").c_str(),
				   (SHADER_SOURCE_PATH + "FragmentShader4.frag").c_str());
		//kinda fragile, but at least the debug and release builds work

	//rendering example
	//vertex data that will be copied into the vertex buffer
	std::vector<float> recVertices = {
		-0.5f, -0.5f,  0.0f, //bottom left
		 0.5f, -0.5f,  0.0f, //bottom right
		-0.5f,  0.5f,  0.0f, //top left
		 0.5f,  0.5f,  0.0f, //top right
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
		 0.0f,  0.5f, 0.0f,
		 0.5f,  0.0f, 0.0f,
		 0.0f, -0.5f, 0.0f,
		-0.5f,  0.0f, 0.0f,
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
	square.create(VertexDataShape::Pos3d, GL_STATIC_DRAW);

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

		//glBindVertexArray(VAO_r); // we bind the VAO that we want to use for drawing
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
		rectangle.draw(GL_TRIANGLES);
		//top triangle
		shader2.use();
		shader2.setUniformi("isInverted", 1); //bool uniform can take in an int
		topTriangle.draw(GL_TRIANGLES);
		
		//bottom triangle
		shader3.use();
		shader3.setUniformf("hOffset", 0.0f);
		bottomTriangle.draw(GL_TRIANGLES);

		//middle square
		shader4.use();
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
}
