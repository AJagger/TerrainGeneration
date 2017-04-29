#pragma once
#include "Renderer.h"
#include "Renderer/nclgl/Mesh.h"
#include "HeightMapGenerator.h"


class World
{
public:
	World();
	~World();

	static const int CHUNK_ARRAY_COUNT = 4;
	void GenerateWorldThroughCombination();
	void GenerateSingleHeightmapSimplex();
	void GenerateSingleHeightmapDS();
	void RenderWorld(Renderer *renderer);
	void RenderSingleMesh(Renderer *renderer);

protected:
	void GenerateMeshesFromWorld();
	HeightMapGenerator::SurroundingHeightmaps GenerateSurroundingHeightmaps(int xChunkValue, int zChunkValue);

	float** chunkHeightmaps[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};
	Mesh* chunkMeshes[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};
	Mesh* waterLevelMeshes[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};
};

