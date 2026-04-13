#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "StructureGenerator.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int width, height;

GLuint renderingProgram;
glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

double mouseX, mouseY;
//bool mouseClicked = false;
bool mouseHeld = false;
bool holdingBox = false;
glm::ivec2 heldBoxTile(-1, -1);  // which tile the box was picked up from
glm::ivec2 hoveredTile(-1, -1);  // tile the mouse is currently over

glm::vec3 cameraPos;

struct Tile {
	bool occupied = false;
	bool hovered = false;
};

const int GRID_SIZE = 20;
Tile grid[GRID_SIZE][GRID_SIZE];

glm::ivec2 getRaycastTile(double mouseX, double mouseY,
	int screenW, int screenH,
	glm::mat4 projection,
	glm::mat4 view,
	glm::vec3 cameraPos)
{
	// Step 1: Screen → NDC
	float ndcX = (2.0f * mouseX) / screenW - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / screenH; // flip Y

	// Step 2: NDC → view space
	glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
	glm::vec4 rayView = glm::inverse(projection) * rayClip;
	rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f); // direction

	// Step 3: View → world space
	glm::vec3 rayWorld = glm::normalize(
		glm::vec3(glm::inverse(view) * rayView)
	);

	// Step 4: Intersect with Y=0 plane
	// Avoid divide by zero (ray parallel to ground)
	if (abs(rayWorld.y) < 1e-6f)
		return glm::ivec2(-1, -1); // no hit

	float t = -cameraPos.y / rayWorld.y;

	// Only accept hits in front of camera
	if (t < 0)
		return glm::ivec2(-1, -1);

	glm::vec3 hitPoint = cameraPos + t * rayWorld;

	int tileX = (int)floor(hitPoint.x + GRID_SIZE / 2.0f);
	int tileZ = (int)floor(hitPoint.z + GRID_SIZE / 2.0f);

	return glm::ivec2(tileX, tileZ);
}

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Mesh ground = generateGridMesh(GRID_SIZE, 1.0f);
	Mesh cube = generateCubeMesh(0.0f, 0.0f, 0.0f, 1.0f);

	renderingProgram = Utils::createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

	// Set up VAO/VBO for ground
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, ground.vertices.size() * sizeof(float), ground.vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ground.indices.size() * sizeof(unsigned int), ground.indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Set up VAO/VBO for cube
	unsigned int cubeVBO, cubeVAO, cubeEBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, cube.vertices.size() * sizeof(float), cube.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indices.size() * sizeof(unsigned int), cube.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	grid[0][0].occupied = true;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(renderingProgram);

		// Camera position (must match lookAt)
		cameraPos = glm::vec3(0.0f, 30.0f, 15.0f);

		// Matrix setup
		glfwGetFramebufferSize(window, &width, &height);
		projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		viewMatrix = glm::lookAt(
			glm::vec3(0.0f, 30.0f, 15.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		// Mouse picking
		glfwGetCursorPos(window, &mouseX, &mouseY);
		hoveredTile = getRaycastTile(mouseX, mouseY, width, height,
			projectionMatrix, viewMatrix, cameraPos);
		bool validTile = hoveredTile.x >= 0 && hoveredTile.x < GRID_SIZE &&
			hoveredTile.y >= 0 && hoveredTile.y < GRID_SIZE;

		if (mouseHeld && !holdingBox) {
			if (validTile && grid[hoveredTile.x][hoveredTile.y].occupied) {
				holdingBox = true;
				heldBoxTile = hoveredTile;
				grid[hoveredTile.x][hoveredTile.y].occupied = false;
			}
		}
		else if (!mouseHeld && holdingBox) {
			if (validTile && !grid[hoveredTile.x][hoveredTile.y].occupied) {
				grid[hoveredTile.x][hoveredTile.y].occupied = true;
			}
			else {
				grid[heldBoxTile.x][heldBoxTile.y].occupied = true;
			}
			holdingBox = false;
			heldBoxTile = glm::ivec2(-1, -1);
		}



		int projectionLocation = glGetUniformLocation(renderingProgram, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		int viewLocation = glGetUniformLocation(renderingProgram, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		int modelLocation = glGetUniformLocation(renderingProgram, "model");
		int colorLocation = glGetUniformLocation(renderingProgram, "color");

		// Draw ground
		glUniform3f(colorLocation, 0.4f, 0.4f, 0.4f);
		modelMatrix = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, ground.indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(cubeVAO);

		// Draw all placed boxes
		glUniform3f(colorLocation, 0.8f, 0.3f, 0.2f);
		for (int x = 0; x < GRID_SIZE; x++) {
			for (int z = 0; z < GRID_SIZE; z++) {
				if (grid[x][z].occupied) {
					float wx = (x - GRID_SIZE / 2.0f) + 0.5f;
					float wz = (z - GRID_SIZE / 2.0f) + 0.5f;
					modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(wx, 0.0f, wz));
					glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
					glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}

		// Draw held box floating above hovered tile
		if (holdingBox && validTile) {
			glUniform3f(colorLocation, 1.0f, 0.8f, 0.2f); // yellow while held
			float wx = (hoveredTile.x - GRID_SIZE / 2.0f) + 0.5f;
			float wz = (hoveredTile.y - GRID_SIZE / 2.0f) + 0.5f;
			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(wx, 0.5f, wz));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouseHeld = true;
		}
		else if (action == GLFW_RELEASE) {
			mouseHeld = false;
		}
	}
}