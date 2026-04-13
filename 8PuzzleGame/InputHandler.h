// InputHandler.h
#pragma once
#include <GLFW/glfw3.h>
#include "Puzzle.h"

class InputHandler
{
public:
	// Call once after window creation. Registers the mouse callback internally.
	static void init(GLFWwindow* window, PuzzleState* state,
		int scrW, int scrH, float spacing, float squareSize);

	// GLFW callback — registered automatically by init()
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:
	static PuzzleState* puzzle;
	static GLFWwindow* window;
	static float       spacing, squareSize;

	// Returns grid index [0–8] of the top-grid square under the cursor, or -1
	static int  hitTest(double mouseX, double mouseY);

	// True if slots a and b are directly up/down/left/right of each other
	static bool adjacent(int a, int b);
};