#pragma once
#include "Renderer/nclgl/ChunkGenerator.h"
#include "Renderer/nclgl/Mesh.h"

class World
{
public:
	World();
	~World();

	static const int CHUNK_ARRAY_COUNT = 4;
	ChunkGenerator::SurroundingHeightmaps GenerateSurroundingHeightmaps(int xChunkValue, int zChunkValue);

protected:
	float** chunkHeightmaps[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};
};

