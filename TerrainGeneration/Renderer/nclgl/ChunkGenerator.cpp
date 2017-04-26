#include "ChunkGenerator.h"
#include "HMCoordinate.h"

using namespace std;

ChunkGenerator::ChunkGenerator(SurroundingHeightmaps sHm, int seed)
{
	//Initialise with generator Seed
	if (seed == NULL) 
	{
		srand(static_cast <unsigned> (time(0)));
	}
	else
	{
		srand(seed);
	}

	surroundingHeightMaps = sHm;
	InitialiseValuesFromExistingChunks();

	//Initialise second heightmap
	heightmap2 = new float*[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++) {
		heightmap2[i] = new float[CHUNK_SIZE];
	}
}

ChunkGenerator::~ChunkGenerator()
{
	for (int i = 0; i < CHUNK_SIZE; i++) {
		delete[] heightmap2[i];
	}

	delete[] heightmap2;
}

void ChunkGenerator::GenerateHeightMap()
{
	InitialiseCorners(TERRAIN_TYPE_PLAIN);
	DiamondSquare(0, 0, 1, TERRAIN_TYPE_PLAIN);
	TransferToPlainHeightmap();

	InitialiseValuesFromExistingChunks();
	InitialiseCorners(TERRAIN_TYPE_ELEVATED);
	DiamondSquare(0, 0, 1, TERRAIN_TYPE_ELEVATED);
	TransferToElevatedHeightmap();

	InitialiseCorners(TERRAIN_MERGE_MAP);
	DiamondSquare(0, 0, 1, TERRAIN_MERGE_MAP);
	TransferToMergeMap();

	MergeMaps();
}

float ** ChunkGenerator::GetHeightMapAsArray()
{
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			heightmap2[y][x] = heightmap[y][x];
		}
	}

	return heightmap2;
}

float ChunkGenerator::GetHeightAt(int xPos, int yPos)
{
	return heightmap[yPos][xPos];
}

float ChunkGenerator::GenerateStartingElevation(int terrainType)
{
	switch (terrainType)
	{
	case 0: //Normal Terrain
		return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_INITIAL_RAND))) - (MAX_INITIAL_RAND / 2));
	case 1: //Elevated Terrain
		return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_INITIAL_RAND))) - (MAX_INITIAL_RAND / 2));
	default:
		return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_INITIAL_RAND))) - (MAX_INITIAL_RAND / 2));
	}
}

float ChunkGenerator::GenerateRandomVariance(int terrainType)
{
	switch (terrainType) 
	{
		case 0: //Normal Terrain
			return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_ITERATIVE_RAND))) - (MAX_ITERATIVE_RAND / 2));
		case 1: //Elevated Terrain
			return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / (MAX_ITERATIVE_RAND*3)))) - (MAX_INITIAL_RAND));
		default:
			return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_ITERATIVE_RAND))) - (MAX_ITERATIVE_RAND / 2));
	}
}

void ChunkGenerator::InitialiseCorners(int terrainType)
{
	if (heightmap[0][0] == NULL || heightmap[0][0] == 0)
		{ heightmap[0][0] = GenerateStartingElevation(terrainType); }
	if (heightmap[CHUNK_SIZE - 1][0] == NULL || heightmap[CHUNK_SIZE - 1][0] == 0)
		{ heightmap[CHUNK_SIZE - 1][0] = GenerateStartingElevation(terrainType); }
	if (heightmap[0][CHUNK_SIZE - 1] == NULL || heightmap[0][CHUNK_SIZE - 1] == 0)
		{ heightmap[0][CHUNK_SIZE - 1] = GenerateStartingElevation(terrainType); }
	if (heightmap[CHUNK_SIZE - 1][CHUNK_SIZE - 1] == NULL || heightmap[CHUNK_SIZE - 1][CHUNK_SIZE - 1] == 0)
		{ heightmap[CHUNK_SIZE - 1][CHUNK_SIZE - 1] = GenerateStartingElevation(terrainType); }
}

void ChunkGenerator::DiamondSquare(int startingX, int startingY, int iteration, int terrainType)
{
	int length = (CHUNK_SIZE - 1) / pow(2, iteration - 1);
	int dampening;
	switch (terrainType) 
	{
		case 0:
			dampening = iteration * iteration;
		case 1:
			dampening = iteration * 0.5;
		default:
			dampening = iteration * iteration;
	}

	if (length < 2) {
		//Algorithm is complete
		//cout << "Algorithm complete, iteration " << iteration << "\n";
	}
	else {
		//Locations of the corners
		HMCoordinate upperLeft = HMCoordinate(startingX, startingY);
		HMCoordinate upperRight = HMCoordinate(startingX + length, startingY);
		HMCoordinate lowerLeft = HMCoordinate(startingX, startingY + length);
		HMCoordinate lowerRight = HMCoordinate(startingX + length, startingY + length);

		//Location of the midpoint
		HMCoordinate midPoint = HMCoordinate(startingX + (length / 2), startingY + (length / 2));

		//Location of the midpoints along the four sides
		HMCoordinate top = HMCoordinate(startingX + (length / 2), startingY);
		HMCoordinate right = HMCoordinate(startingX + length, startingY + (length / 2));
		HMCoordinate bottom = HMCoordinate(startingX + (length / 2), startingY + length);
		HMCoordinate left = HMCoordinate(startingX, startingY + (length / 2));

		//Diamond step
		float diamondValue =
			(heightmap[upperLeft.y][upperLeft.x] +
				heightmap[upperRight.y][upperRight.x] +
				heightmap[lowerLeft.y][lowerLeft.x] +
				heightmap[lowerRight.y][lowerRight.x]) / 4.0f;

		diamondValue += (GenerateRandomVariance(terrainType) / dampening);

		heightmap[midPoint.y][midPoint.x] = static_cast <float> (diamondValue);

		//Square step	//Divide into top/bottom etc so work does not need to be done if value already exists
		float topValue =
			((heightmap[upperLeft.y][upperLeft.x] +
				heightmap[upperRight.y][upperRight.x] +
				heightmap[midPoint.y][midPoint.x]) / 3.0f);
		float rightValue =
			((heightmap[upperRight.y][upperRight.x] +
				heightmap[lowerRight.y][lowerRight.x] +
				heightmap[midPoint.y][midPoint.x]) / 3.0f);
		float bottomValue =
			((heightmap[lowerLeft.y][lowerLeft.x] +
				heightmap[lowerRight.y][lowerRight.x] +
				heightmap[midPoint.y][midPoint.x]) / 3.0f);
		float leftValue =
			((heightmap[upperLeft.y][upperLeft.x] +
				heightmap[lowerLeft.y][lowerLeft.x] +
				heightmap[midPoint.y][midPoint.x]) / 3.0f);

		topValue += (GenerateRandomVariance(terrainType) / dampening);
		rightValue += (GenerateRandomVariance(terrainType) / dampening);
		bottomValue += (GenerateRandomVariance(terrainType) / dampening);
		leftValue += (GenerateRandomVariance(terrainType) / dampening);

		if (heightmap[top.y][top.x] != 0) {
			topValue = (topValue + heightmap[top.y][top.x]) / 2;
		}
		if (heightmap[right.y][right.x] != 0) {
			rightValue = (rightValue + heightmap[right.y][right.x]) / 2;
		}
		if (heightmap[bottom.y][bottom.x] != 0) {
			bottomValue = (bottomValue + heightmap[bottom.y][bottom.x]) / 2;
		}
		if (heightmap[left.y][left.x] != 0) {
			leftValue = (leftValue + heightmap[left.y][left.x]) / 2;
		}

		if (heightmap[top.y][top.x] == 0) 
			{ heightmap[top.y][top.x] = topValue; }
		if (heightmap[right.y][right.x] == 0) 
			{ heightmap[right.y][right.x] = rightValue; }
		if (heightmap[bottom.y][bottom.x] == 0) 
			{ heightmap[bottom.y][bottom.x] = bottomValue; }
		if (heightmap[left.y][left.x] == 0) 
			{ heightmap[left.y][left.x] = leftValue; }

		//Call next iteration of DiamondSquare
		DiamondSquare(upperLeft.x, upperLeft.y, iteration + 1, terrainType);
		DiamondSquare(top.x, top.y, iteration + 1, terrainType);
		DiamondSquare(left.x, left.y, iteration + 1, terrainType);
		DiamondSquare(midPoint.x, midPoint.y, iteration + 1, terrainType);
	}
}

void ChunkGenerator::TransferToPlainHeightmap()
{
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			plainHeightmap[y][x] = heightmap[y][x];
			heightmap[y][x] = 0.0f;
		}
	}
}

void ChunkGenerator::TransferToElevatedHeightmap()
{
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			elevatedHeightmap[y][x] = heightmap[y][x];
			heightmap[y][x] = 0.0f;
		}
	}
}

void ChunkGenerator::TransferToMergeMap()
{
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			mergeMap[y][x] = heightmap[y][x];
			heightmap[y][x] = 0.0f;
		}
	}
}

void ChunkGenerator::InitialiseValuesFromExistingChunks()
{
	if (surroundingHeightMaps.leftHeightMap != NULL)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			heightmap[y][0] = surroundingHeightMaps.leftHeightMap[y][CHUNK_SIZE - 1];
		}
	}

	if (surroundingHeightMaps.topHeightMap != NULL)
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			heightmap[CHUNK_SIZE - 1][x] = surroundingHeightMaps.topHeightMap[0][x];
		}
	}

	if (surroundingHeightMaps.rightHeightMap != NULL)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			heightmap[y][CHUNK_SIZE - 1] = surroundingHeightMaps.rightHeightMap[y][0];
		}
	}

	if (surroundingHeightMaps.bottomHeightMap != NULL)
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			heightmap[0][x] = surroundingHeightMaps.bottomHeightMap[CHUNK_SIZE - 1][x];
		}
	}
}

void ChunkGenerator::MergeMaps()
{
	float scale = MAX_INITIAL_RAND * 0.8f; //0.8f
	float thresholdValue = MAX_INITIAL_RAND - scale;

	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			if (mergeMap[y][x] > thresholdValue)
			{
				if (mergeMap[y][x] > MAX_INITIAL_RAND)
				{
					heightmap[y][x] = elevatedHeightmap[y][x];
				}
				else
				{
					float percentage = (mergeMap[y][x] - thresholdValue) / scale;
					//float percentage = (mergeMap[y][x] - )
					heightmap[y][x] = ((elevatedHeightmap[y][x] * percentage) + (plainHeightmap[y][x] * (1 - percentage)));
				}
			}
			else
			{
				heightmap[y][x] = plainHeightmap[y][x];
			}
		}
	}
}
