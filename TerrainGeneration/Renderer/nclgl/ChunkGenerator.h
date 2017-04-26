#pragma once
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>

class ChunkGenerator
{
public:
	struct SurroundingHeightmaps {
		float **leftHeightMap = NULL;
		float **topHeightMap = NULL;
		float **rightHeightMap = NULL;
		float **bottomHeightMap = NULL;
	};

	ChunkGenerator(SurroundingHeightmaps sHm, int seed = NULL);
	~ChunkGenerator();

	void GenerateHeightMap();
	float** GetHeightMapAsArray();
	float GetHeightAt(int xPos, int yPos);

	static const int CHUNK_SIZE = 1025; //std 129 257
	static const int CHUNK_SIZE_OFFSET = 1024;
	static const int WATER_LEVEL = -20;

private:
	float plainHeightmap[CHUNK_SIZE][CHUNK_SIZE] = {};
	float elevatedHeightmap[CHUNK_SIZE][CHUNK_SIZE] = {};
	float mergeMap[CHUNK_SIZE][CHUNK_SIZE] = {};
	float heightmap[CHUNK_SIZE][CHUNK_SIZE] = {};
	float **heightmap2;
	
	static const int MAX_INITIAL_RAND = 120; //std 60 30 15 --- 60
	static const int MAX_ITERATIVE_RAND = 240; //std 20 60 20 --- 120
	static const int TERRAIN_TYPE_PLAIN = 0;
	static const int TERRAIN_TYPE_ELEVATED = 1;
	static const int TERRAIN_MERGE_MAP = 0;

	SurroundingHeightmaps surroundingHeightMaps;
	float GenerateStartingElevation(int terrainType);
	float GenerateRandomVariance(int terrainType);
	void InitialiseCorners(int terrainType);
	void DiamondSquare(int startingX, int startingY, int iteration, int terrainType);
	void TransferToPlainHeightmap();
	void TransferToElevatedHeightmap();
	void TransferToMergeMap();
	void InitialiseValuesFromExistingChunks();
	void MergeMaps();
};