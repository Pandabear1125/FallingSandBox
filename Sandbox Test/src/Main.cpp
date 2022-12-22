#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/Renderer.h"

#include "Particle.h"

#include <iostream>

/*
 * TODO:
 *		Matter types:
 *			Solids (powders) - done
 *			Solids (hard materials, dont respond to piling, static)
 *			Liquids
 *			Gases
 *		
 *		Materials:
 *			
 */

GLFWwindow* GLInit(float windowWidth, float windowHeight, int vsync);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void showFPS(int& nbFrames, float& lastTime, float dt);
void glfw_error_callback(int code, const char* msg);

float WIN_WIDTH = 160;
float WIN_HEIGHT = 90;
int WIN_SCALE = 10;

ParticleManager world(10, 10, WIN_WIDTH - 20, WIN_HEIGHT - 20);

int mx(0), my(0);
int cx(0), cy(0);

int main()
{
	// GLFW/GLEW Init
	glfwSetErrorCallback(glfw_error_callback);
	GLFWwindow* window = GLInit(WIN_WIDTH * WIN_SCALE, WIN_HEIGHT * WIN_SCALE, -1);

	// Shader Init
	Shader graphicsPolygon("shaders/Shape.vert", "shaders/Polygon.frag");
	//Shader renderStatic("shaders/Static.vert", "shaders/Static.frag");
	//Shader renderDynamic("shaders/Dynamic.vert", "shaders/Dynamic.frag");
	Shader text("shaders/Text.vert", "shaders/Text.frag");

	// Texture Init

	// Shader UBO Init
	UBO MatrixUBO(sizeof(glm::mat4), 0, "Matrices");

	// Add Shaders to Shader UBO
	graphicsPolygon.setUBO(MatrixUBO);
	//renderStatic.setUBO(MatrixUBO);
	//renderDynamic.setUBO(MatrixUBO);
	text.setUBO(MatrixUBO);

	// Graphics Init
	Graphics::GraphicsInit(&graphicsPolygon);
	Graphics::TextInit(&text, "fonts/Arial32pt.png", "fonts/Arial32pt.csv");
	//MultiRender renderer(&renderDynamic);


	// Main App Loop

	int nbFrames = 0;
	float lastTime = (float)glfwGetTime();
	float dt = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		// frame timer update
		float currentFrame = (float)glfwGetTime();
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// render setup
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// this to eventually be replaced by a camera class
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::scale(view, glm::vec3(WIN_SCALE, WIN_SCALE, 0.f));
		glm::mat4 projection = glm::ortho(0.0f, WIN_WIDTH * WIN_SCALE, WIN_HEIGHT * WIN_SCALE, 0.0f);
		glm::mat4 VPmatrix = projection * view;

		// update shader global ubo
		MatrixUBO.SetData(glm::value_ptr(VPmatrix));

		// application logic

		GLenum err;
		while (err = glGetError())
		{
			std::cout << err << std::endl;
		}

		if (!world.paused)
			world.Update(dt, WIN_SCALE);

		Graphics::Print("Mouse: " + std::to_string(mx) + ',' + std::to_string(my) + " (" + std::to_string(cx) + ',' + std::to_string(cy) + ") ID: " + std::to_string(world.GetParticleType(mx, my)), 70, 0, 0.1f);

		world.Draw(WIN_SCALE);

		showFPS(nbFrames, lastTime, dt);

		//// check events and prepare for next frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	std::cout << "Ending Application!" << std::endl;

	return 0;
}

GLFWwindow* GLInit(float windowWidth, float windowHeight, int vsync)
{
	// general init
	std::cout << "Starting init" << std::endl;

	// init GLFW first and configure the window
	if (!glfwInit())
	{
		std::cout << "GLFW init failed!" << std::endl;
		abort();
	}
	else
		std::cout << "GLFW Init Success!" << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// init window
	GLFWwindow* window = glfwCreateWindow((int)windowWidth, (int)windowHeight, "Learning!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		abort();
	}
	else
		std::cout << "Window Init Success!" << std::endl;

	glfwMakeContextCurrent(window);

	// init GLEW now that we have a current context (window)
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
		abort();
	}
	else
		std::cout << "GLEW Init Success!" << std::endl;

	// Set refresh rate | -1: use display refresh | 0: no cap | 1,...: various vsync
	glfwSwapInterval(vsync);

	// Enable depth testing for drawing ontop of things
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable alpha blending for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set viewport
	glViewport(0, 0, (int)windowWidth, (int)windowHeight);
	// Set window resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set input callback functions
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	std::cout << "Init finished" << std::endl;
	// end init

	return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WIN_WIDTH = (float)width / WIN_SCALE;
	WIN_HEIGHT = (float)height / WIN_SCALE;

	std::cout << "Window resized. New size: " << (int)WIN_WIDTH << " by " << (int)WIN_HEIGHT << std::endl;
}

void showFPS(int& nbFrames, float& lastTime, float dt)
{
	float currentTime = (float)glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) { // If last print was more than 1 sec ago
		std::cout << nbFrames << std::endl;
		nbFrames = 0;
		lastTime += 1.0;
	}

	if (dt * 1000 > 17.5f)
		Graphics::Print(dt * 1000.f, 0.f, WIN_HEIGHT - 10.f, 0.25f, 1.f, 1.f, 0.f, 0.f, 2);
	else
		Graphics::Print(dt * 1000.f, 0.f, WIN_HEIGHT - 10.f, 0.25f, 0.f, 1.f, 0.f, 0.f, 2);
}

void glfw_error_callback(int code, const char* msg)
{
	std::cout << "GLFW ERROR: " << code << " | " << msg << std::endl;
}

// called when mouse is moved
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mx = int(xpos) / WIN_SCALE;
	my = int(ypos) / WIN_SCALE;

	cx = mx;
	cy = my;
	world.ToWorldCoord(mx, my);
}

// called when something scrolls
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
}

// called on mouse button actions
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (action == GLFW_PRESS)
	{
		world.isMouseDown = true;
	}
	else if (action == GLFW_RELEASE)
		world.isMouseDown = false;

	std::cout << x << ' ' << y << std::endl;
}

// called on keyboard button actions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		int wx((int)x / WIN_SCALE), wy((int)y / WIN_SCALE);
		world.ToWorldCoord(wx, wy);
		if (world.isLiquid)
			world.SpawnParticle(wx, wy, 10);
		else
			world.SpawnParticle(wx, wy, 20);
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		world.isLiquid = !world.isLiquid;
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		world.debugDraw = !world.debugDraw;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		world.paused = !world.paused;
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		world.Update(0, WIN_SCALE);
	}
}