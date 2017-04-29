#include "stdafx.h"
#include "HeightMapGenerator.h"
#include <cstdlib>
#include <ctime>
#include "Renderer/nclgl/HMCoordinate.h"
#include <cmath>
#include "SimplexNoise.h"


HeightMapGenerator::HeightMapGenerator(SurroundingHeightmaps sHm, int seed)
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

	//Initialise return heightmap
	returnHeightMap = new float*[HEIGHTMAP_SIZE];
	for (int i = 0; i < HEIGHTMAP_SIZE; i++) {
		returnHeightMap[i] = new float[HEIGHTMAP_SIZE];
	}
}


HeightMapGenerator::~HeightMapGenerator()
{
	delete simplexGen;

}

void HeightMapGenerator::GenerateHeightMapUsingCombination()
{
	//Create the basic heightmap (Flat, perlin noise terrain
	InitialiseValuesFromExistingChunks(plainHeightmap);
	InitialiseCorners(plainHeightmap,TERRAIN_TYPE_PLAIN);
	DiamondSquare(plainHeightmap, 0, 0, 1, TERRAIN_TYPE_PLAIN);

	//Create the elevated heightmap (More vertical)
	InitialiseValuesFromExistingChunks(elevatedHeightmap);
	InitialiseCorners(elevatedHeightmap, TERRAIN_TYPE_ELEVATED);
	DiamondSquare(elevatedHeightmap, 0, 0, 1, TERRAIN_TYPE_ELEVATED);

	//Create the merge perlin noise
	InitialiseCorners(mergeMap, TERRAIN_MERGE_MAP);
	DiamondSquare(mergeMap, 0, 0, 1, TERRAIN_MERGE_MAP);

	//Combine the plain and elevated heightmaps using the values contained in the merge map
	MergeMaps();
}

void HeightMapGenerator::GenerateHeightmapSimplex(int frequency, int magnitude)
{
	for (int y = 0; y < HEIGHTMAP_SIZE; y++) {
		for (int x = 0; x < HEIGHTMAP_SIZE; x++) {
			double xf = (double)x / frequency;
			double yf = (double)y / frequency;
			finalHeightmap[y][x] = (float)(simplexGen->simplex(xf, yf, 0)*magnitude - (magnitude/2));
		}
	}
}

void HeightMapGenerator::GenerateHeightmapSingleDS()
{
	InitialiseValuesFromExistingChunks(finalHeightmap);
	InitialiseCorners(finalHeightmap, TERRAIN_TYPE_PLAIN);
	DiamondSquare(finalHeightmap, 0, 0, 1, TERRAIN_TYPE_PLAIN);
}

float ** HeightMapGenerator::GetHeightMapAsArray()
{
	for (int y = 0; y < HEIGHTMAP_SIZE; y++) {
		for (int x = 0; x < HEIGHTMAP_SIZE; x++) {
			returnHeightMap[y][x] = finalHeightmap[y][x];
		}
	}

	return returnHeightMap;
}

float HeightMapGenerator::GetHeightAt(int xPos, int yPos)
{
	return finalHeightmap[yPos][xPos];
}

float HeightMapGenerator::GenerateStartingElevation(int terrainType)
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

float HeightMapGenerator::GenerateRandomVariance(int terrainType)
{
	switch (terrainType)
	{
	case 0: //Normal Terrain
		return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_ITERATIVE_RAND))) - (MAX_ITERATIVE_RAND / 2));
	case 1: //Elevated Terrain
		return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / (MAX_ITERATIVE_RAND * 3)))) - (MAX_INITIAL_RAND));
	default:
		return static_cast <float> (((rand()) / (static_cast <float> (RAND_MAX / MAX_ITERATIVE_RAND))) - (MAX_ITERATIVE_RAND / 2));
	}
}

void HeightMapGenerator::InitialiseCorners(float(&heightmap)[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE], int terrainType)
{
	if (heightmap[0][0] == NULL || heightmap[0][0] == 0)
	{
		heightmap[0][0] = GenerateStartingElevation(terrainType);
	}
	if (heightmap[HEIGHTMAP_SIZE - 1][0] == NULL || heightmap[HEIGHTMAP_SIZE - 1][0] == 0)
	{
		heightmap[HEIGHTMAP_SIZE - 1][0] = GenerateStartingElevation(terrainType);
	}
	if (heightmap[0][HEIGHTMAP_SIZE - 1] == NULL || heightmap[0][HEIGHTMAP_SIZE - 1] == 0)
	{
		heightmap[0][HEIGHTMAP_SIZE - 1] = GenerateStartingElevation(terrainType);
	}
	if (heightmap[HEIGHTMAP_SIZE - 1][HEIGHTMAP_SIZE - 1] == NULL || heightmap[HEIGHTMAP_SIZE - 1][HEIGHTMAP_SIZE - 1] == 0)
	{
		heightmap[HEIGHTMAP_SIZE - 1][HEIGHTMAP_SIZE - 1] = GenerateStartingElevation(terrainType);
	}
}

void HeightMapGenerator::InitialiseValuesFromExistingChunks(float(&heightmap)[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE])
{
	if (surroundingHeightMaps.leftHeightMap != NULL)
	{
		for (int y = 0; y < HEIGHTMAP_SIZE; y++)
		{
			heightmap[y][0] = surroundingHeightMaps.leftHeightMap[y][HEIGHTMAP_SIZE - 1];
		}
	}

	if (surroundingHeightMaps.topHeightMap != NULL)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{
			heightmap[HEIGHTMAP_SIZE - 1][x] = surroundingHeightMaps.topHeightMap[0][x];
		}
	}

	if (surroundingHeightMaps.rightHeightMap != NULL)
	{
		for (int y = 0; y < HEIGHTMAP_SIZE; y++)
		{
			heightmap[y][HEIGHTMAP_SIZE - 1] = surroundingHeightMaps.rightHeightMap[y][0];
		}
	}

	if (surroundingHeightMaps.bottomHeightMap != NULL)
	{
		for (int x = 0; x < HEIGHTMAP_SIZE; x++)
		{
			heightmap[0][x] = surroundingHeightMaps.bottomHeightMap[HEIGHTMAP_SIZE - 1][x];
		}
	}
}

void HeightMapGenerator::DiamondSquare(float(&heightmap)[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE], int startingX, int startingY, int iteration, int terrainType)
{
	int length = (HEIGHTMAP_SIZE_OFFSET) / pow(2, iteration - 1);
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

		if (heightmap[midPoint.y][top.x] == 0)
		{
			heightmap[midPoint.y][midPoint.x] = static_cast <float> (diamondValue);
		}

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

		//if (heightmap[top.y][top.x] != 0) {
		//	topValue = (topValue + heightmap[top.y][top.x]) / 2;
		//}
		//if (heightmap[right.y][right.x] != 0) {
		//	rightValue = (rightValue + heightmap[right.y][right.x]) / 2;
		//}
		//if (heightmap[bottom.y][bottom.x] != 0) {
		//	bottomValue = (bottomValue + heightmap[bottom.y][bottom.x]) / 2;
		//}
		//if (heightmap[left.y][left.x] != 0) {
		//	leftValue = (leftValue + heightmap[left.y][left.x]) / 2;
		//}

		if (heightmap[top.y][top.x] == 0)
		{
			heightmap[top.y][top.x] = topValue;
		}
		if (heightmap[right.y][right.x] == 0)
		{
			heightmap[right.y][right.x] = rightValue;
		}
		if (heightmap[bottom.y][bottom.x] == 0)
		{
			heightmap[bottom.y][bottom.x] = bottomValue;
		}
		if (heightmap[left.y][left.x] == 0)
		{
			heightmap[left.y][left.x] = leftValue;
		}

		//Call next iteration of DiamondSquare
		DiamondSquare(heightmap, upperLeft.x, upperLeft.y, iteration + 1, terrainType);
		DiamondSquare(heightmap, top.x, top.y, iteration + 1, terrainType);
		DiamondSquare(heightmap, left.x, left.y, iteration + 1, terrainType);
		DiamondSquare(heightmap, midPoint.x, midPoint.y, iteration + 1, terrainType);
	}
}

void HeightMapGenerator::MergeMaps()
{
	float scale = MAX_INITIAL_RAND * 0.8f; //0.8f
	float thresholdValue = MAX_INITIAL_RAND - scale;

	for (int y = 0; y < HEIGHTMAP_SIZE; y++) {
		for (int x = 0; x < HEIGHTMAP_SIZE; x++) {
			if (mergeMap[y][x] > thresholdValue)
			{
				if (mergeMap[y][x] > MAX_INITIAL_RAND)
				{
					finalHeightmap[y][x] = elevatedHeightmap[y][x];
				}
				else
				{
					float percentage = (mergeMap[y][x] - thresholdValue) / scale;
					//float percentage = (mergeMap[y][x] - )
					finalHeightmap[y][x] = ((elevatedHeightmap[y][x] * percentage) + (plainHeightmap[y][x] * (1 - percentage)));
				}
			}
			else
			{
				finalHeightmap[y][x] = plainHeightmap[y][x];
			}
		}
	}
}

void HeightMapGenerator::Generate1DNoise(float(&heightmap)[NOISE_1D_SIZE], int startingX, int iteration)
{
	int length = (NOISE_1D_SIZE - 1) / pow(2, iteration - 1);
	int dampening = iteration * iteration;

	if (length < 2) {
		//Algorithm is complete
		//cout << "Algorithm complete, iteration " << iteration << "\n";
	}
	else
	{
		int midPoint = startingX + length / 2;
		int endingX = startingX + length;
		float midPointValue = (noise1D[startingX] + noise1D[endingX]) / 2;
		midPointValue += (GenerateRandomVariance(TERRAIN_TYPE_PLAIN) / dampening);
		noise1D[midPoint] = midPointValue;

		//Call next iteration
		Generate1DNoise(heightmap, startingX, iteration + 1);
		Generate1DNoise(heightmap, midPoint, iteration + 1);
	}
}
