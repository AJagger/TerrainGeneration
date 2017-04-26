#pragma once
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

	void GenerateHeightMapUsingCombination();
	void GenerateHeightMapUsingSeeding();

	float** GetHeightMapAsArray();
	float GetHeightAt(int xPos, int yPos);

	//World Data
	static const int HEIGHTMAP_SIZE = 1025; //std 129 257
	static const int HEIGHTMAP_SIZE_OFFSET = 1024;
	static const int WATER_LEVEL = -20;

private:
	//Stores which method is currently being generated
	bool METHOD_IS_COMBINATION = true;

	//heightmap stores the most recently generated map.
	float heightmap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};

	//Heightmaps used for the Combination method
	float plainHeightmap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};
	float elevatedHeightmap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};
	float mergeMap[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE] = {};

	//Used to return heightmap
	float **returnHeightMap;

	//Chunk Data
	static const int MAX_INITIAL_RAND = 120; //std 60 30 15 --- 60
	static const int MAX_ITERATIVE_RAND = 240; //std 20 60 20 --- 120
	static const int TERRAIN_TYPE_PLAIN = 0;
	static const int TERRAIN_TYPE_ELEVATED = 1;
	static const int TERRAIN_MERGE_MAP = 0;

	//Stores data about surrounding heightmaps if provided
	SurroundingHeightmaps surroundingHeightMaps;

	//Core functions for both implementations
	float GenerateStartingElevation(int terrainType);
	float GenerateRandomVariance(int terrainType);
	void InitialiseCorners(int terrainType);
	void InitialiseValuesFromExistingChunks();
	void DiamondSquare(int startingX, int startingY, int iteration, int terrainType);

	//Core functions for Combination Method
	void TransferToPlainHeightmap();
	void TransferToElevatedHeightmap();
	void TransferToMergeMap();
	void MergeMaps();

	//Core functions for Seeding Method
};

