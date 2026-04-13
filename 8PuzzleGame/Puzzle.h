// Puzzle.h
#pragma once
#include <glm/glm.hpp>
#include <vector>

// Sentinel value used to mark an empty/missing slot

//inline // had to remove inline before const
const glm::vec3 EMPTY_COLOR(-1.0f, -1.0f, -1.0f);

struct PuzzleState
{
	std::vector<glm::vec3> targetColors;   // 9 slots — bottom grid (fixed)
	std::vector<glm::vec3> currentColors;  // 9 slots — top grid (slideable)
	int emptyIndex;                        // which slot in currentColors is the hole
	int targetMissingIndex;                // which slot in targetColors has no square
	bool solved;
};

inline bool checkWin(const PuzzleState& ps)
{
	for (int i = 0; i < 9; i++)
	{
		if (i == ps.targetMissingIndex) continue;
		if (ps.currentColors[i] != ps.targetColors[i]) return false;
	}
	return true;
}