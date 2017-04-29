#pragma once
#include <cmath>
#include "SimplexNoise.h"

class HeightMapGenerator
{
public:
	struct SurroundingHeightmaps {
		float **leftHeightMap = NULL;
		float **topHeightMap = NULL;
		float **rightHeightMap = NULL;
		float **bottomHeightMap = NULL;
	};

	HeightMapGenerator(SurroundingHeightmaps sHm, int seed = NULL);
	~HeightMapGenerator();

	//World Data
	static const int HEIGHTMAP_SIZE = 1025; //std 129 257 //1025
	static const int HEIGHTMAP_SIZE_OFFSET = HEIGHTMAP_SIZE - 1; //1024
	static const int NOISE_1D_SIZE = HEIGHTMAP_SIZE * 2 + 1;
	static const int WATER_LEVEL = -20;

	void GenerateHeightMapUsingCombination();
	void GenerateHeightmapSimplex(int frequency, int magnitude);
	void GenerateHeightmapSingleDS();

	float** GetHeightMapAsArray();
	float GetHeightAt(int xPos, int yPos);

private:
	//Stores which method is currently being generated
	bool METHOD_IS_COMBINATION = true;

	//heightmap stores the final map.
	float finalHeightmap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};

	//Heightmaps used for the Combination method
	float plainHeightmap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};
	float elevatedHeightmap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};
	float mergeMap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};

	//Objects used for Seeding method
	float noise1D[NOISE_1D_SIZE] = {};

	//Used to return heightmap
	float **returnHeightMap;

	//Chunk Data
	static const int MAX_INITIAL_RAND = 120; //std 60 30 15 --- 60 //120
	static const int MAX_ITERATIVE_RAND = 200; //std 20 60 20 --- 120 //240
	static const int TERRAIN_TYPE_PLAIN = 0;
	static const int TERRAIN_TYPE_ELEVATED = 1;
	static const int TERRAIN_MERGE_MAP = 0;

	//Stores data about surrounding heightmaps if provided
	SurroundingHeightmaps surroundingHeightMaps;

	//Core functions for both implementations
	float GenerateStartingElevation(int terrainType);
	float GenerateRandomVariance(int terrainType);
	void InitialiseCorners(float(&heightmap)[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE], int terrainType);
	void InitialiseValuesFromExistingChunks(float(&heightmap)[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE]);
	void DiamondSquare(float(&heightmap)[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE], int startingX, int startingY, int iteration, int terrainType);

	//Core functions for Combination Method
	void MergeMaps();

	//Core functions for Seeding Method
	void Generate1DNoise(float(&heightmap)[NOISE_1D_SIZE], int startingX, int iteration);

	//Core Functions for Simplex Noise
	SimplexNoise *simplexGen = new SimplexNoise();
};

