#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp> 
#include "Utils.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
GLuint renderingProgram;

struct GridState
{
	std::vector<glm::vec3> colors;
	int missingIndex;
};

struct PuzzleState
{
	std::vector<glm::vec3> targetColors; // bottom grid (fixed)
	std::vector<glm::vec3> currentColors; // top grid (moves)
	int emptyIndex;
};

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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	renderingProgram = Utils::createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

	float size = 0.25f;

	float vertices[] = {
		 size,  size, 0.0f,
		 size, -size, 0.0f,
		-size, -size, 0.0f,
		-size,  size, 0.0f
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<int> distIndex(0, 8);

	auto generateGrid = [&](std::mt19937& gen) -> GridState
		{
			GridState grid;

			std::vector<glm::vec3> palette = {
				{1.0f, 0.0f, 0.0f}, // red
				{0.0f, 1.0f, 0.0f}, // green
				{0.0f, 0.0f, 1.0f}, // blue
				{1.0f, 1.0f, 0.0f}, // yellow
				{1.0f, 0.0f, 1.0f}, // magenta
				{0.0f, 1.0f, 1.0f}, // cyan
				{1.0f, 0.5f, 0.0f}, // orange
				{0.5f, 0.0f, 1.0f}  // purple
			};

			std::shuffle(palette.begin(), palette.end(), gen);

			grid.colors = palette;
			grid.missingIndex = distIndex(gen);

			return grid;
		};

	GridState gridA = generateGrid(g);
	GridState gridB = generateGrid(g);

	auto renderGrid = [&](GridState& grid, float zOffset, int modelLoc, int colorLoc)
		{
			float spacing = 0.6f;
			int colorIdx = 0;

			for (int row = 0; row < 3; row++)
			{
				for (int col = 0; col < 3; col++)
				{
					int index = row * 3 + col;

					if (index == grid.missingIndex)
						continue;

					float x = (col - 1) * spacing;
					float y = (1 - row) * spacing;

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(x, y, zOffset));

					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

					glm::vec3 c = grid.colors[colorIdx++];
					glUniform4f(colorLoc, c.r, c.g, c.b, 1.0f);

					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}
			}
		};

	int colorLoc = glGetUniformLocation(renderingProgram, "squareColor");
	int modelLoc = glGetUniformLocation(renderingProgram, "model");
	int modeLoc = glGetUniformLocation(renderingProgram, "renderMode");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(renderingProgram);

		glBindVertexArray(VAO);

		glDisable(GL_BLEND);

		glUniform1i(modeLoc, 0); // striped
		renderGrid(gridA, -0.1f, modelLoc, colorLoc);

		glUniform1i(modeLoc, 1); // solid
		renderGrid(gridB, 0.0f, modelLoc, colorLoc);

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