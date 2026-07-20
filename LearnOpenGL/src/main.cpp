//------------- GLFW + openGL3 ------------- 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//------------- stb -------------
#include <stb/stb_image.h>

//------------- glm -------------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> //glm::to_string

//------------- imgui -------------
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "assimp/Importer.hpp"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Shader.h"
#include "Shape.h"
#include "Texture.h"
#include "Camera.h"
#include "ModelLoader.h"

//TODO: material class
struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int shine;
};

//TODO: light class
struct PointLight { //light from a single point, decreases intensity over distance
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;
};

struct DirLight { //constant light in certain direction, position acts like infinity
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct SpotLight { //mix, position + direction
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float cutoff;
	float outerCutoff;
	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;
};

//window specs
const int WIDTH = 800;
const int HEIGHT = 800;

//colors
const glm::vec4 BG_BLUE{ 0.2f, 0.3f, 0.5f, 1.0f};
const glm::vec4 BORDER{ 0.2f, 0.2f, 0.2f, 1.0f};

glm::vec4 BGColor = BG_BLUE;
glm::vec3 cubeColor{ 1.0f, 0.5f, 0.15f};

//materials
Material cubeMaterial{
	{0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	32
};

//light
PointLight pLight{
	{1.0f, 1.0f, 1.0f},
	{0.2f, 0.2f, 0.2f},
	{0.5f, 0.5f, 0.5f},
	{1.0f, 1.0f, 1.0f},
	1.0f, 0.5f, 0.7f
};

PointLight pLight2{
	{2.0f, 1.0f, 2.0f},
	{0.2f, 0.2f, 0.2f},
	{0.5f, 0.5f, 0.5f},
	{1.0f, 1.0f, 1.0f},
	1.0f, 0.5f, 0.7f
};

DirLight dLight{
	{0.2f, -1.0f, 0.2f},
	{0.2f, 0.2f, 0.2f},
	{0.5f, 0.5f, 0.5f},
	{1.0f, 1.0f, 1.0f}
};

SpotLight sLight{
	{5.0f, 2.0f, 5.0f},
	{0.2f, -1.0f, 0.2f},
	{0.2f, 0.2f, 0.2f},
	{0.5f, 0.5f, 0.5f},
	{1.0f, 1.0f, 1.0f},
	12.5f, 15.0f,
	1.0f, 0.045f, 0.0075f
};

glm::vec3 emmisionColor{0.0f, 0.2f, 0.0f};

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
void glfwErrorCallback(int error_code, const char* description);

//core
GLFWwindow* Init();
void setGLFWEventCallbacks(GLFWwindow* window);
void processInput(GLFWwindow* window, float deltaTime);
void enterDebug();

float texInterp = 0.0f;
float FOV = 45.0f;

//camera 
Camera camera{glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f) ,-30.0f, -135.0f, 3.0f};

float lightOrbitRadius = 1.0f;
float lightOrbitSpeed = 1.0f;

//cursor properties
double mouseLastX = WIDTH / 2, mouseLastY = HEIGHT / 2;
bool firstMouse = true;

//delta time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mouseEnabled = true;
bool shouldExit = false;

//sphere stuff
void createSphere(int sectorCount, int stackCount, float radius, glm::vec3 origin, std::vector<float>& vertices, std::vector<unsigned int>& indices);

int main() {
	
	GLFWwindow* window = Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Control
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init("#version 330");

	//create the shader program that uses the vertex and fragment shaders in the shaders folder
	Shader cubeShader((SHADER_SOURCE_PATH + "NormalVertexShader.vert").c_str(),
				      (SHADER_SOURCE_PATH + "NormalFragmentShader.frag").c_str());
	Shader lightShader((SHADER_SOURCE_PATH + "LightVertexShader.vert").c_str(),
		               (SHADER_SOURCE_PATH + "LightFragmentShader.frag").c_str());
	Shader sphereShader((SHADER_SOURCE_PATH + "TextureVertexShader.vert").c_str(),
					    (SHADER_SOURCE_PATH + "TextureFragmentShader.frag").c_str());

	//texture stuff
	Texture texture1((ASSETS_PATH + "textures/cool_cat.png").c_str());
	Texture texture2((ASSETS_PATH + "textures/urara_ballin.png").c_str());
	Texture globeTexture((ASSETS_PATH + "textures/globe.png").c_str());
	Texture perlin((ASSETS_PATH + "textures/perlin_noise.png").c_str());
	Texture containerTexture((ASSETS_PATH + "textures/container.png").c_str());
	Texture containerSpecular((ASSETS_PATH + "textures/container_specular_improved.png").c_str());
	Texture containerEmmision((ASSETS_PATH + "textures/wood_emmision.png").c_str());

	//---------------------------------------------------------------------------------

	//------------------ model loading ------------------

	std::vector<float> cubeVertices;
	std::vector<unsigned int> cubeIndices;
	std::vector<float> sphereVertices;
	std::vector<unsigned int> sphereIndices;

	cubeVertices = {
		//positions         //normals          //texture coordinates
		//back
		 0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  1.0f,0.0f,
		-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  1.0f,1.0f,
		 0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.0f,1.0f,

	    //front
	    -0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f,0.0f,
		 0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,0.0f,
		 0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,1.0f,
		-0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f,1.0f,
	   
	    //left
	    -0.5f,-0.5f,-0.5f, -1.0f, 0.0f, 0.0f,  0.0f,0.0f,
		-0.5f,-0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  1.0f,0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  1.0f,1.0f,
		-0.5f, 0.5f,-0.5f, -1.0f, 0.0f, 0.0f,  0.0f,1.0f,
	   
	    //right
	     0.5f,-0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f,0.0f,
		 0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  1.0f,0.0f,
		 0.5f, 0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  1.0f,1.0f,
		 0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f,1.0f,
	   
	    //bottom
	     0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f,  0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  1.0f,1.0f,
		 0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  0.0f,1.0f,
	   
	     //top
	    -0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  0.0f,0.0f,
		 0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f,0.0f,
		 0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f,1.0f,
		-0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  0.0f,1.0f,
	};

	cubeIndices = {
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

	createSphere(20, 20, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), sphereVertices, sphereIndices);

	//------------------ shape data ------------------
	Shape cube(cubeVertices, cubeIndices);
	cube.create(PosNormTex3d, GL_STATIC_DRAW);

	//texture
	cubeShader.use();
	cubeShader.setUniformi("material.diffuse", 0);
	cubeShader.setUniformi("material.specular", 1);
	cubeShader.setUniformi("material.emmision", 2);


	Shape sphere(sphereVertices, sphereIndices);
	sphere.create(PosNormTex3d, GL_STATIC_DRAW);

	//texture
	sphereShader.use();
	sphereShader.setUniformi("lightMap", 0);

	Shape lightBox(sphereVertices, sphereIndices);
	//reuse the cube's buffers since they are already sent to the gpu
	lightBox.setVBO(sphere.getVBO());
	lightBox.setEBO(sphere.getEBO());
	lightBox.create(PosNormTex3d, GL_STATIC_DRAW);

	glm::vec3 cubePositions[100];
	for (int i = 0; i < 10; ++i) {
		for(int j = 0; j <10; ++j){
			cubePositions[i * 10 + j] = glm::vec3{i*1.0f, 0.0f, j*1.0f};
		}
	}

	//for wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//----------------------- main loop -----------------------
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (mouseEnabled) 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)

		//input
		processInput(window, deltaTime);

		//rendering
		glClearColor(BGColor.r, 
					 BGColor.g, 
					 BGColor.b, 
					 BGColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//------------- LIGHT
		pLight.position.x = (float)std::cos(glfwGetTime() * lightOrbitSpeed) * lightOrbitRadius;
		pLight.position.z = (float)std::sin(glfwGetTime() * lightOrbitSpeed) * lightOrbitRadius;

		lightShader.use();
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, pLight.position);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		lightShader.setUniformMat4("model", glm::value_ptr(lightModel));
		
		glm::mat4 lightView = glm::mat4(1.0f);
		lightView = camera.getViewMatrix();
		lightShader.setUniformMat4("view", glm::value_ptr(lightView));

		glm::mat4 lightProjection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		lightShader.setUniformMat4("projection", glm::value_ptr(lightProjection));

		//normalizing doesn't really make sense here but it looks better for visualization so ill keep it
		lightShader.setUniformVec3("color", glm::normalize(pLight.ambient+pLight.diffuse+pLight.specular));
		
		lightBox.draw(GL_TRIANGLES);

		lightShader.use();
		lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, pLight2.position);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		lightShader.setUniformMat4("model", glm::value_ptr(lightModel));

		lightBox.draw(GL_TRIANGLES);

		//------------ CUBE
		cubeShader.use();
		//coordinate transformation pipeline (MVP pipeline)
		//model matrix: local -> world
		glm::mat4 cubeModel(1.0f);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.0f, 0.0f));
		cubeShader.setUniformMat4("model", glm::value_ptr(cubeModel));

		//view matrix: world -> view
		glm::mat4 cubeView(1.0f);
		cubeView = camera.getViewMatrix();
		cubeShader.setUniformMat4("view", glm::value_ptr(cubeView));

		//projection matrix: view -> clip
		glm::mat4 cubeProjection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		cubeShader.setUniformMat4("projection", glm::value_ptr(cubeProjection));
		
		//light properties
		//can be wrapped in an apply#Light(#Light) function
		cubeShader.setUniformVec3("pLight.position", pLight.position);
		cubeShader.setUniformVec3("pLight.ambient", pLight.ambient);
		cubeShader.setUniformVec3("pLight.diffuse", pLight.diffuse);
		cubeShader.setUniformVec3("pLight.specular", pLight.specular);
		cubeShader.setUniformf("pLight.constant", pLight.constant);
		cubeShader.setUniformf("pLight.linear", pLight.linear);
		cubeShader.setUniformf("pLight.quadratic", pLight.quadratic);

		cubeShader.setUniformVec3("pLight2.position", pLight2.position);
		cubeShader.setUniformVec3("pLight2.ambient", pLight2.ambient);
		cubeShader.setUniformVec3("pLight2.diffuse", pLight2.diffuse);
		cubeShader.setUniformVec3("pLight2.specular", pLight2.specular);
		cubeShader.setUniformf("pLight2.constant", pLight2.constant);
		cubeShader.setUniformf("pLight2.linear", pLight2.linear);
		cubeShader.setUniformf("pLight2.quadratic", pLight2.quadratic);

		cubeShader.setUniformVec3("dLight.direction", dLight.direction);
		cubeShader.setUniformVec3("dLight.ambient", dLight.ambient);
		cubeShader.setUniformVec3("dLight.diffuse", dLight.diffuse);
		cubeShader.setUniformVec3("dLight.specular", dLight.specular);

		cubeShader.setUniformVec3("sLight.position", camera.Position);
		cubeShader.setUniformVec3("sLight.direction", camera.Front);
		cubeShader.setUniformVec3("sLight.ambient", sLight.ambient);
		cubeShader.setUniformVec3("sLight.diffuse", sLight.diffuse);
		cubeShader.setUniformVec3("sLight.specular", sLight.specular);
		cubeShader.setUniformf("sLight.cutoff", std::cos(glm::radians(sLight.cutoff)));
		cubeShader.setUniformf("sLight.outerCutoff", std::cos(glm::radians(sLight.outerCutoff)));
		cubeShader.setUniformf("sLight.constant", sLight.constant);
		cubeShader.setUniformf("sLight.linear", sLight.linear);
		cubeShader.setUniformf("sLight.quadratic", sLight.quadratic);

		//material
		cubeShader.setUniformi("material.shine", cubeMaterial.shine);
		cubeShader.setUniformVec3("emmisionColor", emmisionColor);

		cubeShader.setUniformVec3("cameraPos", camera.Position);
		cubeShader.setUniformf("time", (float)glfwGetTime());

		containerTexture.setUnit(0);
		containerSpecular.setUnit(1);
		containerEmmision.setUnit(2);
		for (int i = 0; i < 100;++i) {
			cubeModel = glm::mat4{ 1.0f };
			cubeModel = glm::translate(cubeModel, cubePositions[i]);
			cubeShader.setUniformMat4("model", glm::value_ptr(cubeModel));
			cube.draw(GL_TRIANGLES);
		}
		
		//imgui
		ImGui::Begin("Debug window");
		ImGui::Checkbox("Enable cursor", &mouseEnabled);
		if (ImGui::Button("Close Window"))
			glfwSetWindowShouldClose(window, 1);
		ImGui::ColorEdit4("Background Color", glm::value_ptr(BGColor));
		//light
		ImGui::Text("Light Properties");
		ImGui::SliderFloat3("Point Light Position", glm::value_ptr(pLight.position), -2.0f, 2.0f);
		ImGui::ColorEdit3("Point Light Ambient Color", glm::value_ptr(pLight.ambient));
		ImGui::ColorEdit3("Point Light Diffuse Color", glm::value_ptr(pLight.diffuse));
		ImGui::ColorEdit3("Point Light Specular Color", glm::value_ptr(pLight.specular));
		ImGui::SliderFloat3("Point Light Attenuation Coefficients", &pLight.constant, 0.0f, 1.0f); //relies on structs being contiguous in memory
		ImGui::SliderFloat("Point Light Orbit Radius", &lightOrbitRadius, 0.5f, 3.0f);
		ImGui::SliderFloat("Point Light Orbit Speed", &lightOrbitSpeed, 0.0f, 5.0f);

		ImGui::SliderFloat("Spotlight Inner Cutoff", &sLight.cutoff, 10.0f, 20.0f);
		ImGui::SliderFloat("Spotlight Outer Cutoff", &sLight.outerCutoff, 10.0f, 20.0f);

		ImGui::SliderFloat3("Directional Light Direction", glm::value_ptr(dLight.direction), -10.0f, 10.0f);
		ImGui::ColorEdit3("Direction Light Ambient Color", glm::value_ptr(dLight.ambient));
		ImGui::ColorEdit3("Direction Light Diffuse Color", glm::value_ptr(dLight.diffuse));
		ImGui::ColorEdit3("Direction Light Specular Color", glm::value_ptr(dLight.specular));
		
		//cube
		ImGui::Text("Cube Properties");
		ImGui::ColorEdit3("Cube Emmision Color", glm::value_ptr(emmisionColor));
		ImGui::SliderInt("Cube Shine", &cubeMaterial.shine, 0, 256);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/*GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "GL error: " << err << std::endl;
		}*/

		//check and poll events and swap frame buffers
		glfwSwapBuffers(window);
	}

	//imgui shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

	//hides the cursor and sets it to the middle of the window, should be toggleable if i want to use imgui
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
		//glfwSetWindowShouldClose(window, 1);
		enterDebug();
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
	glfwSetWindowShouldClose(window, 1);
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

	if(!mouseEnabled)
		camera.processMouseMovement((float)xoffset, (float)yoffset);
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (!mouseEnabled)
		camera.processMouseScroll((float)yoffset);
}

void glfwErrorCallback(int error_code, const char* description) {
	std::cerr << "GLFW ERROR " << error_code << ": " << description << std::endl;
}

void enterDebug() {
	mouseEnabled = true;
}

void setGLFWEventCallbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, OnFrameBufferResize);
	glfwSetWindowCloseCallback(window, OnWindowClose);
	glfwSetCursorPosCallback(window, onCursorMove);
	glfwSetScrollCallback(window, onMouseScroll);
	glfwSetErrorCallback(glfwErrorCallback);
}

void createSphere(int sectorCount, int stackCount, float radius, glm::vec3 origin, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
	float sectorStep = 2 * (float)M_PI / (float)sectorCount;
	float stackStep = (float)M_PI / (float)stackCount;

	float sectorAngle = 0.0;
	float stackAngle = -(float)M_PI_2;
	for (int i = 0; i <= stackCount; ++i) {
		sectorAngle = 0.0f;
		for (int j = 0; j <= sectorCount; ++j) {
			float x = cos(stackAngle) * sin(sectorAngle) * radius + origin.x;
			float y = sin(stackAngle) * radius + origin.y;
			float z = cos(stackAngle) * cos(sectorAngle) * radius + origin.z;

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			//normals
			glm::vec3 normal = glm::normalize(glm::vec3(x, y, z) - origin);
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			//texture
			float tx = (float)j / sectorCount;
			float ty = (float)i / stackCount;
			vertices.push_back(tx);
			vertices.push_back(ty);

			sectorAngle += sectorStep;
		}
		stackAngle += stackStep;
	}

	//i need to understand this part more
	for (int i = 0; i < stackCount; ++i) {
		int k1 = i * (sectorCount + 1);
		int k2 = k1 + sectorCount + 1;
		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			//exclude first sector 
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			//exclude last sector
			if (i != sectorCount - 1) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}
