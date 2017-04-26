#pragma once
#include "OGLRenderer.h"
#include "../nclgl/ChunkGenerator.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateTriangleStrip();
	static Mesh* GenerateChunk(float ** heightmap, int chunkZ, int chunkX);
	static Mesh* GenerateWaterChunk(int chunkZ, int chunkX);
	static const int GRID_SIZE = 1;

protected:
	void BufferData();
	static Vector4 DetermineColour(float elevation);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;
};

