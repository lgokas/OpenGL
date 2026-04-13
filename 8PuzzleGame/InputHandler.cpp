// InputHandler.cpp
#include "InputHandler.h"
#include <iostream>
#include <cmath>

// Static member definitions
PuzzleState* InputHandler::puzzle = nullptr;
int          InputHandler::screenW = 800;
int          InputHandler::screenH = 600;
float        InputHandler::spacing = 0.6f;
float        InputHandler::squareSize = 0.25f;

void InputHandler::init(GLFWwindow* window, PuzzleState* state,
	int scrW, int scrH, float sp, float sq)
{
	puzzle = state;
	screenW = scrW;
	screenH = scrH;
	spacing = sp;
	squareSize = sq;
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

int InputHandler::hitTest(double mouseX, double mouseY)
{
	// Convert window-space cursor position to NDC.
	// No view/projection matrix is used in the shaders, so NDC == world space.
	float ndcX = (float)(mouseX / screenW) * 2.0f - 1.0f;
	float ndcY = -(float)(mouseY / screenH) * 2.0f + 1.0f;

	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			int idx = row * 3 + col;

			// Skip the hole — it can't be clicked
			if (idx == puzzle->emptyIndex) continue;

			// Centre of this square in NDC (matches the translate in renderGrid)
			float cx = (col - 1) * spacing;
			float cy = (1 - row) * spacing;

			if (ndcX >= cx - squareSize && ndcX <= cx + squareSize &&
				ndcY >= cy - squareSize && ndcY <= cy + squareSize)
			{
				return idx;
			}
		}
	}
	return -1;
}

bool InputHandler::adjacent(int a, int b)
{
	int dr = std::abs(a / 3 - b / 3);
	int dc = std::abs(a % 3 - b % 3);
	return (dr + dc) == 1;   // exactly one step up/down/left/right
}

void InputHandler::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;
	if (!puzzle || puzzle->solved) return;

	if (puzzle->animation.active) return;

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	int clicked = hitTest(mouseX, mouseY);
	if (clicked == -1) return;

	if (adjacent(clicked, puzzle->emptyIndex))
	{
		// ── CHANGED: fill in animation instead of swapping immediately ────────
		int fromRow = clicked / 3, fromCol = clicked % 3;
		int toRow = puzzle->emptyIndex / 3, toCol = puzzle->emptyIndex % 3;

		TileAnimation& anim = puzzle->animation;
		anim.active = true;
		anim.fromIndex = clicked;
		anim.toIndex = puzzle->emptyIndex;
		anim.progress = 0.0f;
		anim.startPos = { (fromCol - 1) * spacing, (1 - fromRow) * spacing };
		anim.endPos = { (toCol - 1) * spacing, (1 - toRow) * spacing };
		// The logical swap (currentColors / emptyIndex) happens in main.cpp
		// once progress reaches 1.0.
	}

}