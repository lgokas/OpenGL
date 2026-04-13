// Puzzle.h
#pragma once
#include <glm/glm.hpp>
#include <vector>


const glm::vec3 EMPTY_COLOR(-1.0f, -1.0f, -1.0f); // removed inline from the suggestion as it wasn't letting compile

// ── NEW: tweak this to control slide speed ────────────────────────────────────
//   Units: full-tile-lengths per second.  4.0 ≈ 0.25 s per move.
constexpr float SLIDE_SPEED = 4.0f; // removed inline from the suggestion as it wasn't letting compile

// ── NEW: per-move animation state ─────────────────────────────────────────────
struct TileAnimation
{
	bool      active = false;
	int       fromIndex = -1;     // logical slot the tile is leaving
	int       toIndex = -1;     // logical slot (hole) it is entering
	float     progress = 0.0f;   // 0 → 1
	glm::vec2 startPos = {};     // world-space centre at progress == 0
	glm::vec2 endPos = {};     // world-space centre at progress == 1
};

struct PuzzleState
{
	std::vector<glm::vec3> targetColors;
	std::vector<glm::vec3> currentColors;
	int emptyIndex;
	int targetMissingIndex;
	bool solved;

	TileAnimation animation;     // ← NEW
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