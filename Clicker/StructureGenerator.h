#pragma once
#include <vector>

struct Mesh {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

Mesh generateGridMesh(int gridSize, float tileSize);
Mesh generateCubeMesh(float x, float y, float z, float size);