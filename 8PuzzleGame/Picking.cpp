#include "Picking.h"

glm::vec2 screenToWorld(double mouseX, double mouseY,
	int screenW, int screenH)
{
	// Step 1: pixel → NDC
	float ndcX = (float)(mouseX / screenW) * 2.0f - 1.0f;
	float ndcY = -((float)(mouseY / screenH) * 2.0f - 1.0f); // flip Y

	// Step 2: NDC → world
	// With identity projection & view (your current setup), NDC == world.
	// To extend to 3D: multiply by inverse(projection * view) here.
	return glm::vec2(ndcX, ndcY);
}

bool pointInSquare(glm::vec2 worldPoint, const Square& sq)
{
	return worldPoint.x >= sq.center.x - sq.half &&
		worldPoint.x <= sq.center.x + sq.half &&
		worldPoint.y >= sq.center.y - sq.half &&
		worldPoint.y <= sq.center.y + sq.half;
}

std::string squareName(int row, int col)
{
	const char* rowNames[] = { "bottom", "middle", "top" };
	const char* colNames[] = { "left",   "centre", "right" };
	return std::string(rowNames[row]) + "-" + colNames[col];
}