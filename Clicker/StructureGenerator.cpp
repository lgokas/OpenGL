#include "StructureGenerator.h"

Mesh generateGridMesh(int gridSize, float tileSize) {
	Mesh mesh;
	for (int z = 0; z <= gridSize; z++) {
		for (int x = 0; x <= gridSize; x++) {
			float px = (x - gridSize / 2.0f) * tileSize;
			float pz = (z - gridSize / 2.0f) * tileSize;
			mesh.vertices.push_back(px);
			mesh.vertices.push_back(0.0f);
			mesh.vertices.push_back(pz);
		}
	}
	for (int z = 0; z < gridSize; z++) {
		for (int x = 0; x < gridSize; x++) {
			int topLeft = z * (gridSize + 1) + x;
			int topRight = topLeft + 1;
			int bottomLeft = (z + 1) * (gridSize + 1) + x;
			int bottomRight = bottomLeft + 1;
			mesh.indices.push_back(topLeft);
			mesh.indices.push_back(bottomLeft);
			mesh.indices.push_back(topRight);
			mesh.indices.push_back(topRight);
			mesh.indices.push_back(bottomLeft);
			mesh.indices.push_back(bottomRight);
		}
	}
	return mesh;
}

Mesh generateCubeMesh(float x, float y, float z, float size) {
	Mesh mesh;
	float h = size / 2.0f;

	// 8 corners of the cube centred at (x, y+h, z)
	// so the base sits at Y = y
	mesh.vertices = {
		x - h, y,     z - h,  // 0 bottom
		x + h, y,     z - h,  // 1
		x + h, y,     z + h,  // 2
		x - h, y,     z + h,  // 3
		x - h, y + size,z - h,  // 4 top
		x + h, y + size,z - h,  // 5
		x + h, y + size,z + h,  // 6
		x - h, y + size,z + h   // 7
	};

	mesh.indices = {
		0,1,2, 2,3,0,  // bottom
		4,5,6, 6,7,4,  // top
		0,1,5, 5,4,0,  // front
		2,3,7, 7,6,2,  // back
		0,3,7, 7,4,0,  // left
		1,2,6, 6,5,1   // right
	};

	return mesh;
}