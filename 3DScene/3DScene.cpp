// Dwaine Brannon
// CS 330
// 3D scene

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<iostream>
#include <vector>
#include "Camera.h"
#include "icosphere/icosphere/src/Icosphere.h"

// GLM Libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <corecrt_math_defines.h>

const float PI = 3.14159f;
const int numSides = 30;
const float height = 1.0f;
const float radius = 0.5f;

// texture variable
GLuint textureId; // Plane
GLuint textureId2; // Switch Dock
GLuint textureId3; // Chapstick
GLuint textureId4; // Switch
GLuint textureId5; // Pokeball

static glm::vec2 gUVScale(1.0f, 1.0f);
GLint texWrapMode = GL_REPEAT;

// unnamed namespace
namespace {
	const char* const WINDOW_TITLE = "Dwaine's Scene";

	// Window Size
	const int WINDOW_WIDTH = 800;
		const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh {
		GLuint vertexArrayObject; //cube
		GLuint vertexBufferObject; //cube
		GLuint elementBufferObject; //cube
		GLuint numOfIndices; // Number of indices in mesh 
		GLuint nVertices; // num of vertices in mesh
		GLuint vertexArrayObject2; // plane
		GLuint vertexBufferObject2; //plane
		GLuint elementBufferObject2; //plane
		GLuint switchVAO;
		GLuint switchVBO;
		GLuint chapstickVAO;
		GLuint chapstickVBO;
		GLuint pokeballVAO;
		GLuint pokeballVBO;
		std::vector<float> interleavedData; 
		GLuint pokeballIBO;
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	//Mesh Data
	GLMesh mesh1; // cube mesh
	GLMesh mesh2; // plane mesh
	GLMesh switchMesh; // Switch Mesh
	GLMesh chapstickMesh;
	GLMesh pokeballMesh; 
	// Shader Program 
	GLuint programId1;
	GLuint programId2;

	// Camera 
	glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */

bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
//Mesh functions
void createChapstickMesh(GLMesh& mesh);
void createPokeballMesh(GLMesh& mesh);
void cubeMesh(GLMesh& mesh);
void planeMesh(GLMesh& mesh);
void nintendoSwitchMesh(GLMesh& mesh); // simple cube for nintendo switch 
void UDestroyMesh(GLMesh& mesh);
bool UCreateShaderProgram(const char* vtxhaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void URender();

// Mouse Input Functions 
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
bool UCreateTexture(const char* filename, GLuint& textureId, const char* filename2, GLuint& textureId2, const char* filename3, GLuint& textureId3, const char* filename4, GLuint& textureId4, const char* filename5, GLuint& textureId5);
// Camera Variables
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_HEIGHT / 2.0f;
float lastY = WINDOW_WIDTH / 2.0f;
bool firstMouse = true;

// Subject Position and scale
glm::vec3 cubePosition = glm::vec3(1.0f, -0.64f, 0.0f);
glm::vec3 cubeScale = glm::vec3(1.5f, 1.0f, 1.0f);

glm::vec3 planePosition = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 planeScale = glm::vec3(6.0f, 6.0f, 6.0f);

glm::vec3 switchPosition = glm::vec3(-1.0f, -0.64f, 1.0f);
glm::vec3 switchScale = glm::vec3(1.0f, 1.0f, 0.5f);

glm::vec3 chapstickScale = glm::vec3(0.5f, 0.5f, 1.0f) * glm::vec3(-0.5f);
glm::vec3 chapstickPosition = glm::vec3(-3.0f, -0.74f, 1.0f);

glm::vec3 pokeballScale = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 pokeballPosition = glm::vec3(3.0f, -0.74, 1.0f); 

// pyramid and light color
glm::vec3 objectColor = glm::vec3(0.6f, 0.5f, 0.75f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); 

// Lighting Variables
glm::vec3 lightPosition = glm::vec3(1.0f, 0.5f, 3.0f);
glm::vec3 lightScale = glm::vec3(1.0f, 1.0f, 1.0f);

//lamp animation
bool gIsLampOrbiting = true;


// Shaders
// --------------------------------
// Cube shader
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 vPosition;"
"layout(location = 1) in vec3 normal;"
"layout(location = 2) in vec2 textureCoordinate;"
"out vec3 vertexNormal;"
"out vec3 vertexFragmentPos;"
"out vec2 vertexTextureCoordinate;"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(vPosition, 1.0);\n" // Transforms vertices into clip coordinates
"vertexFragmentPos = vec3(model * vec4(vPosition, 1.0f));"  // Gets fragment / pixel pposition in world space onlyu
"vertexNormal = mat3(transpose(inverse(model))) * normal;" // get normal vectors in world space and exclude normal translation properties
"vertexTextureCoordinate = textureCoordinate;\n"
"}\n";


// Fragment Shader Program Source Code for shapes
const char* fragmentShaderSource =
"#version 330 core\n"
"in vec3 vertexNormal;"
"in vec3 vertexFragmentPos;"
"in vec2 vertexTextureCoordinate;"
"uniform sampler2D uTexture;"
"out vec4 fragColor;"
"uniform vec2 uvScale;"
"uniform vec3 objectColor;"
"uniform vec3 lightColor;"
"uniform vec3 lightPos;"
"uniform vec3 viewPosition;"
"void main()\n"
"{\n" // Phone lighting model calculations to generate ambient, diffuse, and specular components
"float ambientStrength = 0.8f;" // Set ambient or global lighting strength
"vec3 ambient = ambientStrength * lightColor;" // Generates ambient light color
"vec3 norm = normalize(vertexNormal);" // NNormalize vectors to 1 unit
"vec3 lightDirection = normalize(lightPos - vertexFragmentPos);" // Calculate distande between light source and fragments/pixels on pyramid
"float impact = max(dot(norm, lightDirection), 0.0);" // calculate the diffuse impact by generating dot product of normal and light
"vec3 diffuse = impact * lightColor;" // Generate diffuse light color
"float specularIntensity = 0.8f;" // set sspecular light strength 
"float highlightSize = 16.0f;"
"vec3 viewDir = normalize(viewPosition - vertexFragmentPos);"
"vec3 reflectDir = reflect(-lightDirection, norm);"
"float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);"
"vec3 specular = specularIntensity * specularComponent * lightColor;"
"vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);"
"vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;"
"fragColor = vec4(phong, 1.0);\n"
"}\n";

// Lamp Shader source code
const char* lampVertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 vPosition;" // VAP position 0 for vertex position data
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(vPosition, 1.0f);\n"
"}\n";

// lamp frag shader source
const char* lampFragmentShaderSource =
"#version 330 core\n"
"out vec4 fragColor;" // for outgoing lamp color to the gpu
"void main()\n"
"{\n"
"fragColor = vec4(1.0f);\n" //  set color to white 
"}\n";

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}


// Main function. 
int main(int argc, char* argv[]) {
	

	if (!UInitialize(argc, argv, &gWindow)) {
		return EXIT_FAILURE;
	}

	// Create the Mesh 
	cubeMesh(mesh1); // Calls function to create the VBO 
	planeMesh(mesh2); // Plane mesh
	nintendoSwitchMesh(switchMesh); // nintendo switch
	createChapstickMesh(chapstickMesh); // chapstick mesh
	createPokeballMesh(pokeballMesh); // pokeball Mesh 
	// Create Shader Program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, programId1)) {
		return EXIT_FAILURE;
}
	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, programId2)) {
		return EXIT_FAILURE;
	}
	
	
	// load texture
	const char* texFileName = "PokemonSwitchDock.jpg";
	const char* texFileName2 = "DresserTex.jpg";
	const char* texFileName3 = "MYSWITCH.jpg";
	const char* texFileName4 = "ChapstickTex.jpg";
	const char* texFileName5 = "POKEBALL.jpg";
	if (!UCreateTexture(texFileName, textureId, texFileName2, textureId2, texFileName3, textureId3, texFileName4, textureId4, texFileName5, textureId5)) {
		std::cout << "Failed to load texture " << texFileName << std::endl;
		return EXIT_FAILURE;
	}
	// enable z-depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// game loop
	// ---------
	while (!glfwWindowShouldClose(gWindow)) {
		// per frame timing
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// input
		UProcessInput(gWindow);

		// Clear the background
		glClearColor(0.2f, 0.3, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// call render function
		URender();
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.

		glfwPollEvents();
	}
	// Release Mesh datta
	UDestroyMesh(mesh1); 
	UDestroyMesh(mesh2);
	UDestroyMesh(switchMesh);
	UDestroyMesh(chapstickMesh);
	UDestroyMesh(pokeballMesh);

	// release shader program
	UDestroyShaderProgram(programId1);
	UDestroyShaderProgram(programId2);
	exit(EXIT_SUCCESS); // Terminates the program successfully

}

// Initialize GLFW, GLEW and Create a window

bool UInitialize(int argc, char* argv[], GLFWwindow** window) {

	// initialize and configure GLFW window and specify openGL version)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, FL_TRUE);
#endif 
	// GLFW Window Creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, mousePositionCallback);
	glfwSetScrollCallback(*window, mouseScrollCallback);
	glfwSetMouseButtonCallback(*window, mouseButtonCallback);

	// tell glfw to capture our mouse
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{

		static const float cameraSpeed = 1.5f; // adjust accordingly

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		float cameraOffset = cameraSpeed * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.ProcessKeyboard(LEFT, deltaTime);

		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboard(RIGHT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.ProcessKeyboard(UP, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.ProcessKeyboard(DOWN, deltaTime);
		}

		// Pause and resume lamp orbiting
		static bool isLKeyDown = false;
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !gIsLampOrbiting)
			gIsLampOrbiting = true;
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && gIsLampOrbiting)
			gIsLampOrbiting = false;

		//just trying to debug stuff 
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			std::vector<float> interleavedData = chapstickMesh.interleavedData;

			// Print out the interleaved data
			for (int i = 0; i < interleavedData.size(); i++) {
				std::cout << interleavedData[i] << " ";
			}
		}
	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// Render
void URender() {

	
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);
	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// activate the vao 
	glBindVertexArray(mesh1.vertexArrayObject);

	glUseProgram(programId1);
	// MVP Matrix setup 
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	// Render Cube
	modelMatrix = glm::translate(cubePosition) * glm::scale(cubeScale);

	glUniformMatrix4fv(glGetUniformLocation(programId1, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	GLint objectColorLoc = glGetUniformLocation(programId1, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(programId1, "lightColor");
	GLint lightPositionLoc = glGetUniformLocation(programId1, "lightPos");
	GLint viewPositionLoc = glGetUniformLocation(programId1, "viewPosition");
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	const glm::vec3 cameraPosition = camera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// for texture
	GLint UVScaleLoc = glGetUniformLocation(programId1, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_TRIANGLES, 0, mesh1.nVertices);

	// activate the vao for plane
	glBindVertexArray(mesh2.vertexArrayObject2);

	// Render Plane
	modelMatrix = glm::translate(planePosition) * glm::scale(planeScale);
	glUseProgram(programId1);
	glUniformMatrix4fv(glGetUniformLocation(programId1, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	objectColorLoc = glGetUniformLocation(programId1, "objectColor");
	lightColorLoc = glGetUniformLocation(programId1, "lightColor");
	lightPositionLoc = glGetUniformLocation(programId1, "lightPos");
	viewPositionLoc = glGetUniformLocation(programId1, "viewPosition");
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//texture for plane
	GLint UVScaleLoc2 = glGetUniformLocation(programId1, "uvScale");
	glUniform2fv(UVScaleLoc2, 1, glm::value_ptr(gUVScale));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId2);

	glDrawArrays(GL_TRIANGLES, 0, mesh2.nVertices);

	// Draw Switch
	// VAO 
	glBindVertexArray(switchMesh.switchVAO);
	modelMatrix = glm::translate(switchPosition) * glm::scale(switchScale);
	glUseProgram(programId1);
	glUniformMatrix4fv(glGetUniformLocation(programId1, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	objectColorLoc = glGetUniformLocation(programId1, "objectColor");
	lightColorLoc = glGetUniformLocation(programId1, "lightColor");
	lightPositionLoc = glGetUniformLocation(programId1, "lightPos");
	viewPositionLoc = glGetUniformLocation(programId1, "viewPosition");
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//texture for switch
	GLint UVScaleLoc3 = glGetUniformLocation(programId1, "uvScale");
	glUniform2fv(UVScaleLoc3, 1, glm::value_ptr(gUVScale));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId3);

	glDrawArrays(GL_TRIANGLES, 0, switchMesh.nVertices);

	// Draw Chapstick
	// VAO 
	glm::mat4 rotationMatrix =  glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glBindVertexArray(chapstickMesh.chapstickVAO);
	modelMatrix = glm::translate(chapstickPosition) * rotationMatrix * glm::scale(chapstickScale);
	glUseProgram(programId1);
	glUniformMatrix4fv(glGetUniformLocation(programId1, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	objectColorLoc = glGetUniformLocation(programId1, "objectColor");
	lightColorLoc = glGetUniformLocation(programId1, "lightColor");
	lightPositionLoc = glGetUniformLocation(programId1, "lightPos");
	viewPositionLoc = glGetUniformLocation(programId1, "viewPosition");
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//texture for chapstick
	GLint UVScaleLoc4 = glGetUniformLocation(programId1, "uvScale");
	glUniform2fv(UVScaleLoc4, 1, glm::value_ptr(gUVScale));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId4);

	glDrawArrays(GL_TRIANGLES, 0, chapstickMesh.nVertices);
	
	// Draw Pokeball
	// VAO 
	glBindVertexArray(pokeballMesh.pokeballVAO);
	modelMatrix = glm::translate(pokeballPosition) * glm::scale(pokeballScale);
	glUseProgram(programId1);
	glUniformMatrix4fv(glGetUniformLocation(programId1, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programId1, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	objectColorLoc = glGetUniformLocation(programId1, "objectColor");
	lightColorLoc = glGetUniformLocation(programId1, "lightColor");
	lightPositionLoc = glGetUniformLocation(programId1, "lightPos");
	viewPositionLoc = glGetUniformLocation(programId1, "viewPosition");
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//texture forpokeball
	GLint UVScaleLoc5 = glGetUniformLocation(programId1, "uvScale");
	glUniform2fv(UVScaleLoc5, 1, glm::value_ptr(gUVScale));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId5);

	glDrawArrays(GL_TRIANGLES, 0, pokeballMesh.nVertices);

	glBindVertexArray(0);
	glUseProgram(0);
}
// Texture FUnciton
bool UCreateTexture(const char* filename, GLuint& textureId, const char* filename2, GLuint& textureId2, const char* filename3, GLuint& textureId3, const char* filename4, GLuint& textureId4, const char* filename5, GLuint& textureId5) {
	
	// load first texture
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image) {

		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);


		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << channels << " channels" << std::endl;
			return false;
		}
		stbi_image_free(image);

		int width2, height2, channels2;
		unsigned char* image2 = stbi_load(filename2, &width2, &height2, &channels2, 0);

		if (image2) {
			flipImageVertically(image2, width2, height2, channels2);

			glGenTextures(1, &textureId2);
			glBindTexture(GL_TEXTURE_2D, textureId2);

			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
			else if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);

			else
			{
				std::cout << "Not implemented to handle image with " << channels2 << " channels" << std::endl;
				return false;
			}

			stbi_image_free(image2);
		}
		int width3, height3, channels3;
		unsigned char* image3 = stbi_load(filename3, &width3, &height3, &channels3, 0);

		if (image3) {
			flipImageVertically(image3, width3, height3, channels3);

			glGenTextures(1, &textureId3);
			glBindTexture(GL_TEXTURE_2D, textureId3);

			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
			else if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, image3);

			else
			{
				std::cout << "Not implemented to handle image with " << channels3 << " channels" << std::endl;
				return false;
			}

			stbi_image_free(image3);
		}

		int width4, height4, channels4;
		unsigned char* image4 = stbi_load(filename4, &width4, &height4, &channels4, 0);

		if (image4) {
			flipImageVertically(image4, width4, height4, channels4);

			glGenTextures(1, &textureId4);
			glBindTexture(GL_TEXTURE_2D, textureId4);

			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width4, height4, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);
			else if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width4, height4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image4);

			else
			{
				std::cout << "Not implemented to handle image with " << channels4 << " channels" << std::endl;
				return false;
			}

			stbi_image_free(image4);
		}

		int width5, height5, channels5;
		unsigned char* image5 = stbi_load(filename5, &width5, &height5, &channels5, 0);

		if (image5) {
			flipImageVertically(image5, width5, height5, channels5);

			glGenTextures(1, &textureId5);
			glBindTexture(GL_TEXTURE_2D, textureId5);

			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width5, height5, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
			else if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width5, height5, 0, GL_RGBA, GL_UNSIGNED_BYTE, image5);

			else
			{
				std::cout << "Not implemented to handle image with " << channels5 << " channels" << std::endl;
				return false;
			}

			stbi_image_free(image5);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}
	// error loading image
	return false;
}

// mesh for cube object
void cubeMesh(GLMesh& mesh)
{
	// Specifies Normalized Device Coordinates for the plane that will represent the nintendo switch dock
	GLfloat verts[] = {
		 // positions			normals						textures

		-0.5f, -0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,			 0.0f,  0.0f,
		 0.5f, -0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,			1.0f,  0.0f,
		 0.5f,  0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,			1.0f,  1.0f,
		 0.5f,  0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
		-0.5f,  0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		 0.0f,  1.0f,
		-0.5f, -0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		 0.0f,  0.0f,

		-0.5f, -0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 0.0f,  0.0f,
		 0.5f, -0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 1.0f,  0.0f,
		 0.5f,  0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 1.0f,  1.0f,
		 0.5f,  0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 1.0f,  1.0f,
		-0.5f,  0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 0.0f,  1.0f,
		-0.5f, -0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 0.0f,  0.0f,

		-0.5f,  0.35f,  0.2f,	-1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
		-0.5f,  0.35f, -0.2f,	 -1.0f,  0.0f,  0.0f,		 1.0f,  1.0f,
		-0.5f, -0.35f, -0.2f,	 -1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
		-0.5f, -0.35f, -0.2f,	 -1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
		-0.5f, -0.35f,  0.2f,	 -1.0f,  0.0f,  0.0f,		 0.0f,  0.0f,
		-0.5f,  0.35f,  0.2f,	-1.0f,  0.0f,  0.0f,		 1.0f,  0.0f,

		 0.5f,  0.35f,  0.2f,	  1.0f,  0.0f,  0.0f,		  1.0f,  0.0f,
		 0.5f,  0.35f, -0.2f,	 1.0f,  0.0f,  0.0f,		 1.0f,  1.0f,
		 0.5f, -0.35f, -0.2f,	 1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
		 0.5f, -0.35f, -0.2f,	 1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
		 0.5f, -0.35f,  0.2f,	 1.0f,  0.0f,  0.0f,		 0.0f,  0.0f,
		 0.5f,  0.35f,  0.2f,	 1.0f,  0.0f,  0.0f,		 1.0f,  0.0f,

		-0.5f, -0.35f, -0.2f,	  0.0f, -1.0f,  0.0f,		  0.0f,  1.0f,
		 0.5f, -0.35f, -0.2f,	  0.0f, -1.0f,  0.0f,		 1.0f,  1.0f,
		 0.5f, -0.35f,  0.2f,	  0.0f, -1.0f,  0.0f,		  1.0f,  0.0f,
		 0.5f, -0.35f,  0.2f,	  0.0f, -1.0f,  0.0f,		 1.0f,  0.0f,
		-0.5f, -0.35f,  0.2f,	  0.0f, -1.0f,  0.0f,		 0.0f,  0.0f,
		-0.5f, -0.35f, -0.2f,	 0.0f, -1.0f,  0.0f,		0.0f,  1.0f,

		-0.5f,  0.35f, -0.2f,	  0.0f,  1.0f,  0.0f,		  0.0f,  1.0f,
		 0.5f,  0.35f, -0.2f,	 0.0f,  1.0f,  0.0f,		 1.0f,  1.0f,
		 0.5f,  0.35f,  0.2f,	 0.0f,  1.0f,  0.0f,		1.0f,  0.0f,
		 0.5f,  0.35f,  0.2f,	  0.0f,  1.0f,  0.0f,		 1.0f,  0.0f,
		-0.5f,  0.35f,  0.2f,	 0.0f,  1.0f,  0.0f,		0.0f,  0.0f,
		-0.5f,  0.35f, -0.2f,	  0.0f,  1.0f,  0.0f,		 0.0f,  1.0f
		
	};
	
	glGenVertexArrays(1, &mesh.vertexArrayObject); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vertexArrayObject);

	const GLuint floatsPerPosition = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerPosition + floatsPerNormal + floatsPerUV));

	// Create VBO and bind it 
	glGenBuffers(1, &mesh.vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferObject); // activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Calculate the stride
	GLint stride = sizeof(float) * (floatsPerPosition + floatsPerNormal + floatsPerUV);

	// Creates the Vertex Attribute Pointer for positions
	glVertexAttribPointer(0, floatsPerPosition, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	// Attribute 1: vertex normals
	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerPosition));
	glEnableVertexAttribArray(1);
	// texture
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerPosition + floatsPerNormal)));
	glEnableVertexAttribArray(2); 

}

// desk mesh
void planeMesh(GLMesh& mesh) {


	// Vertices for the plane that represents my desk
	GLfloat verts2[] = {
		// Positions			// Normals			Texture
		// First triangle
		-1.0f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,												  // bottom left
		 1.0f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,												 // bottom right
		 1.0f, 0.0f,  0.5f,		0.0f, 1.0f, 0.0f, 	1.0f, 1.0f,												  // top right

		 // Second triangle
		 -1.0f, 0.0f, -0.5f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,												   // bottom left
		 1.0f, 0.0f,  0.5f,		0.0f, 1.0f, 0.0f, 	1.0f, 1.0f,													  // top right
		 -1.0f, 0.0f,  0.5f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f								  // top left
	};

	const GLuint floatsPerPlanePos = 3;
	const GLuint floatsPerPlaneNormal = 3;
	const GLuint floatsPerPlaneUV = 2;

	mesh.nVertices = sizeof(verts2) / (sizeof(verts2[0]) * (floatsPerPlanePos + floatsPerPlaneNormal + floatsPerPlaneUV));

	glGenVertexArrays(1, &mesh.vertexArrayObject2);
	glBindVertexArray(mesh.vertexArrayObject2);



	// Create VBO and bind it

	glGenBuffers(1, &mesh.vertexBufferObject2);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferObject2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts2), verts2, GL_STATIC_DRAW);

	// Stride 
	GLint stride = sizeof(float) * (floatsPerPlanePos + floatsPerPlaneNormal + floatsPerPlaneUV);
	// Create Vertex Attribute Pointer for positions of plane
	glVertexAttribPointer(0, floatsPerPlanePos, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	// Attribute 1: vertex normals
	glVertexAttribPointer(1, floatsPerPlaneNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerPlanePos));
	glEnableVertexAttribArray(1);
	// texture
	glVertexAttribPointer(2, floatsPerPlaneUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerPlanePos + floatsPerPlaneNormal)));
	glEnableVertexAttribArray(2);


}

void nintendoSwitchMesh(GLMesh& mesh) {
	// Points for switch. object will just be black 
	GLfloat verts[] = {
		// positions			 Normals					textures

	   -0.9f, -0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		 0.0f,  0.0f,
		0.9f, -0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		1.0f,  0.0f,
		0.9f,  0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
		0.9f,  0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
	   -0.9f,  0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		 0.0f,  1.0f,
	   -0.9f, -0.35f, -0.2f,	 0.0f,  0.0f, -1.0f,		 0.0f,  0.0f,

	   -0.9f, -0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 0.0f,  0.0f,
		0.9f, -0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 1.0f,  0.0f,
		0.9f,  0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 1.0f,  1.0f,
		0.9f,  0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 1.0f,  1.0f,
	   -0.9f,  0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 0.0f,  1.0f,
	   -0.9f, -0.35f,  0.2f,	 0.0f,  0.0f,  1.0f,		 0.0f,  0.0f,

	   -0.9f,  0.35f,  0.2f,	-1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
	   -0.9f,  0.35f, -0.2f,	 -1.0f,  0.0f,  0.0f,		 1.0f,  1.0f,
	   -0.9f, -0.35f, -0.2f,	 -1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
	   -0.9f, -0.35f, -0.2f,	 -1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
	   -0.9f, -0.35f,  0.2f,	 -1.0f,  0.0f,  0.0f,		 0.0f,  0.0f,
	   -0.9f,  0.35f,  0.2f,	-1.0f,  0.0f,  0.0f,		 1.0f,  0.0f,

		0.9f,  0.35f,  0.2f,	  1.0f,  0.0f,  0.0f,		  1.0f,  0.0f,
		0.9f,  0.35f, -0.2f,	 1.0f,  0.0f,  0.0f,		 1.0f,  1.0f,
		0.9f, -0.35f, -0.2f,	 1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
		0.9f, -0.35f, -0.2f,	 1.0f,  0.0f,  0.0f,		 0.0f,  1.0f,
		0.9f, -0.35f,  0.2f,	 1.0f,  0.0f,  0.0f,		 0.0f,  0.0f,
		0.9f,  0.35f,  0.2f,	 1.0f,  0.0f,  0.0f,		 1.0f,  0.0f,

	   -0.9f, -0.35f, -0.2f,	  0.0f, -1.0f,  0.0f,		  0.0f,  1.0f,
		0.9f, -0.35f, -0.2f,	  0.0f, -1.0f,  0.0f,		 1.0f,  1.0f,
		0.9f, -0.35f,  0.2f,	  0.0f, -1.0f,  0.0f,		  1.0f,  0.0f,
		0.9f, -0.35f,  0.2f,	  0.0f, -1.0f,  0.0f,		 1.0f,  0.0f,
	   -0.9f, -0.35f,  0.2f,	  0.0f, -1.0f,  0.0f,		 0.0f,  0.0f,
	   -0.9f, -0.35f, -0.2f,	 0.0f, -1.0f,  0.0f,		0.0f,  1.0f,

	   -0.9f,  0.35f, -0.2f,	  0.0f,  1.0f,  0.0f,		  0.0f,  1.0f,
		0.9f,  0.35f, -0.2f,	 0.0f,  1.0f,  0.0f,		 1.0f,  1.0f,
		0.9f,  0.35f,  0.2f,	 0.0f,  1.0f,  0.0f,		1.0f,  0.0f,
		0.9f,  0.35f,  0.2f,	  0.0f,  1.0f,  0.0f,		 1.0f,  0.0f,
	   -0.9f,  0.35f,  0.2f,	 0.0f,  1.0f,  0.0f,		0.0f,  0.0f,
	   -0.9f,  0.35f, -0.2f,	  0.0f,  1.0f,  0.0f,		 0.0f,  1.0f

	};
	glGenVertexArrays(1, &mesh.switchVAO); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.switchVAO);

	const GLuint floatsPerPosition = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerPosition + floatsPerNormal + floatsPerUV));

	// Create VBO and bind it 
	glGenBuffers(1, &mesh.switchVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.switchVBO); // activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// Calculate the stride
	GLint stride = sizeof(float) * (floatsPerPosition + floatsPerNormal + floatsPerUV);

	// Creates the Vertex Attribute Pointer for positions
	glVertexAttribPointer(0, floatsPerPosition, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	// Attribute 1: vertex normals
	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerPosition));
	glEnableVertexAttribArray(1);
	// texture
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerPosition + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void createChapstickMesh(GLMesh& mesh) {
	
	unsigned int numSegments = 6;
	float radius = 0.5f;
	float height = 1.0f;
	std::vector <float> verts;
	std::vector<float> normals;
	std::vector<float> texCoords;

	float angleStep = 2 * PI / numSegments;

	// Define the vertices for top and bottom faces of the cylinder
	for (int i = 0; i < numSegments; ++i) {
		float angle = i * angleStep;
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		// top face
		verts.push_back(x);
		verts.push_back(y);
		verts.push_back(height / 2);

		normals.push_back(0);
		normals.push_back(0);
		normals.push_back(1);

		texCoords.push_back(x + 0.5f);
		texCoords.push_back(y + 0.5f);
		// Bottom face
		verts.push_back(x);
		verts.push_back(y);
		verts.push_back(-height / 2);

		normals.push_back(0);
		normals.push_back(0);
		normals.push_back(-1);

		texCoords.push_back(x + 0.5f);
		texCoords.push_back(y + 0.5f);

	}

	// Define the vertices for the side faces of the cylinder
	for (int i = 0; i < numSegments; ++i) {
		float angle = i * angleStep;
		float x1 = radius * cos(angle);
		float y1 = radius * sin(angle);
		float x2 = radius * cos(angle + angleStep);
		float y2 = radius * sin(angle + angleStep);

		// Calculate the normalized normals for the side faces
		float nx1 = x1 / radius;
		float ny1 = y1 / radius;
		float nx2 = x2 / radius;
		float ny2 = y2 / radius;

		// First triangle
		verts.push_back(x1);
		verts.push_back(y1);
		verts.push_back(height / 2);

		normals.push_back(nx1);
		normals.push_back(ny1);
		normals.push_back(0);

		texCoords.push_back(i / (float)numSegments);
		texCoords.push_back(0);

		verts.push_back(x1);
		verts.push_back(y1);
		verts.push_back(-height / 2);

		normals.push_back(nx1);
		normals.push_back(ny1);
		normals.push_back(0);

		texCoords.push_back(i / (float)numSegments);
		texCoords.push_back(1);

		verts.push_back(x2);
		verts.push_back(y2);
		verts.push_back(height / 2);

		normals.push_back(nx2);
		normals.push_back(ny2);
		normals.push_back(0);

		texCoords.push_back((i + 1) / (float)numSegments);
		texCoords.push_back(0);

		// Second triangle
		verts.push_back(x2);
		verts.push_back(y2);
		verts.push_back(height / 2);

		normals.push_back(nx2);
		normals.push_back(ny2);
		normals.push_back(0);

		texCoords.push_back((i + 1)/ (float)numSegments);
		texCoords.push_back(0);


		verts.push_back(x1);
		verts.push_back(y1);
		verts.push_back(-height / 2);

		normals.push_back(nx1);
		normals.push_back(ny1);
		normals.push_back(0);

		texCoords.push_back(i / (float)numSegments);
		texCoords.push_back(1);

		verts.push_back(x2);
		verts.push_back(y2);
		verts.push_back(-height / 2);

		normals.push_back(nx2);
		normals.push_back(ny2);
		normals.push_back(0);

		texCoords.push_back((i + 1) / (float)numSegments);
		texCoords.push_back(1);
	}

	std::vector<float> interleavedData;
	for (int i = 0; i < verts.size() / 3; i++) {
		// Add vertex position
		interleavedData.push_back(verts[i * 3]);
		interleavedData.push_back(verts[i * 3 + 1]);
		interleavedData.push_back(verts[i * 3 + 2]);
		// Add vertex normal
		interleavedData.push_back(normals[i * 3]);
		interleavedData.push_back(normals[i * 3 + 1]);
		interleavedData.push_back(normals[i * 3 + 2]);
		// Add vertex texture coordinate
		interleavedData.push_back(texCoords[i * 2]);
		interleavedData.push_back(texCoords[i * 2 + 1]);
	}

	glGenVertexArrays(1, &mesh.chapstickVAO); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.chapstickVAO);

	const GLuint floatsPerPosition = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	int numTriangles = interleavedData.size() / 3;

	mesh.nVertices = numTriangles * 3;

	// Create VBO and bind it
	glGenBuffers(1, &mesh.chapstickVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.chapstickVBO); // activates the buffer
	glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);
	// Calculate the stride
	GLint stride = sizeof(float) * (floatsPerPosition + floatsPerNormal + floatsPerUV);

	// Specify the position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
	glEnableVertexAttribArray(0);

	// Specify the normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Specify the texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void createPokeballMesh(GLMesh& mesh) {

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	float radius = 0.5f;
	unsigned int ySegments = 50;
	unsigned int xSegments = 50;

	for (unsigned int y = 0; y <= ySegments; ++y) {
		for (unsigned int x = 0; x <= xSegments; ++x) {
			float xSegment = (float)x / (float)xSegments;
			float ySegment = (float)y / (float)ySegments;
			float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI) * radius;
			float yPos = std::cos(ySegment * M_PI) * radius;
			float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI) * radius;

			vertices.push_back(xPos);
			vertices.push_back(yPos);
			vertices.push_back(zPos);

			normals.push_back(xPos / radius);
			normals.push_back(yPos / radius);
			normals.push_back(zPos / radius);

			texCoords.push_back(xSegment);
			texCoords.push_back(ySegment);
		}
	}

	std::vector<unsigned int> indices;
	for (unsigned int y = 0; y < ySegments; ++y) {
		for (unsigned int x = 0; x < xSegments; ++x) {
			indices.push_back((y + 1) * (xSegments + 1) + x);
			indices.push_back(y * (xSegments + 1) + x);
			indices.push_back(y * (xSegments + 1) + x + 1);

			indices.push_back((y + 1) * (xSegments + 1) + x);
			indices.push_back(y * (xSegments + 1) + x + 1);
			indices.push_back((y + 1) * (xSegments + 1) + x + 1);
		}
	}

	// Generate VAO, VBO, and EBO
	glGenVertexArrays(1, &mesh.pokeballVAO);
	glBindVertexArray(mesh.pokeballVAO);

	glGenBuffers(1, &mesh.pokeballVBO);
	glGenBuffers(1, &mesh.pokeballIBO);

	// Combine vertices, normals, and texCoords into a single vector
	std::vector<float> combinedData;
	for (size_t i = 0; i < vertices.size() / 3; i++) {
		combinedData.push_back(vertices[i * 3]);
		combinedData.push_back(vertices[i * 3 + 1]);
		combinedData.push_back(vertices[i * 3 + 2]);

		combinedData.push_back(normals[i * 3]);
		combinedData.push_back(normals[i * 3 + 1]);
		combinedData.push_back(normals[i * 3 + 2]);

		combinedData.push_back(texCoords[i * 2]);
		combinedData.push_back(texCoords[i * 2 + 1]);
	}

	// Bind vertex data to vbo
	glGenVertexArrays(1, &mesh.pokeballVAO);
	glBindVertexArray(mesh.pokeballVAO);
	int floatsPerPosition = 3;
	int floatsPerNormal = 3;
	int floatsPerUV = 2;

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, mesh.pokeballVBO);
	glBufferData(GL_ARRAY_BUFFER, combinedData.size() * sizeof(float), combinedData.data(), GL_STATIC_DRAW);

	// Bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.pokeballIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Vertex attribute pointers
	GLsizei stride = (3 + 3 + 2) * sizeof(float);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	mesh.nVertices = static_cast<unsigned int>(indices.size());

}
void UDestroyMesh(GLMesh& mesh) {
	glDeleteVertexArrays(1, &mesh.vertexArrayObject);
	glDeleteBuffers(1, &mesh.vertexBufferObject);
	glDeleteVertexArrays(1, &mesh.vertexArrayObject2);
	glDeleteBuffers(1, &mesh.vertexBufferObject2);
	glDeleteVertexArrays(1, &mesh.chapstickVAO);
	glDeleteBuffers(1, &mesh.chapstickVBO);
	glDeleteVertexArrays(1, &mesh.switchVAO);
	glDeleteBuffers(1, &mesh.switchVBO);
	glDeleteVertexArrays(1, &mesh.pokeballVAO);
	glDeleteVertexArrays(1, &mesh.pokeballVBO);
	glDeleteBuffers(1, &mesh.pokeballIBO);

}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader

	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attatched compiled shaders to the shader program 
	glCompileShader(vertexShaderId);
	glCompileShader(fragmentShaderId);

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glLinkProgram(programId); // Use shader program

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}


/* GLSL Error Checking Definitions */
void PrintShaderCompileError(GLuint shader)
{
	int len = 0;
	int chWritten = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWritten, log);
		std::cout << "Shader Compile Error: " << log << std::endl;
		free(log);
	}
}


void PrintShaderLinkingError(int prog)
{
	int len = 0;
	int chWritten = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(prog, len, &chWritten, log);
		std::cout << "Shader Linking Error: " << log << std::endl;
		free(log);
	}
}


bool IsOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "glError: " << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

/* GLSL Error Checking Definitions End Here */

// glfw: Whenever the mouse moves, this callback is called.
// -------------------------------------------------------
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: Whenever the mouse scroll wheel scrolls, this callback is called.
// ----------------------------------------------------------------------
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
// glfw: Handle mouse button events.
// --------------------------------
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			std::cout << "Left mouse button pressed" << std::endl;
		else
			std::cout << "Left mouse button released" << std::endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			std::cout << "Middle mouse button pressed" << std::endl;
		else
			std::cout << "Middle mouse button released" << std::endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			std::cout << "Right mouse button pressed" << std::endl;
		else
			std::cout << "Right mouse button released" << std::endl;
	}
	break;

	default:
		std::cout << "Unhandled mouse button event" << std::endl;
		break;
	}
}