#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Square {
	int row, col;
	glm::vec2 center;
	float half;
};

struct AppState {
	std::vector<Square>* squares;
	int screenW, screenH;
	int selectedRow = -1;
	int selectedCol = -1;
	int bottomEmptyPos = 8;  // which board position is empty on the bottom grid
	glm::vec4 grid1Colors[8];
	glm::vec4 grid2Colors[8];
};

glm::vec2 screenToWorld(double mouseX, double mouseY, int screenW, int screenH);
bool pointInSquare(glm::vec2 worldPoint, const Square& sq);
std::string squareName(int row, int col);