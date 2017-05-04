#include "stdafx.h"
#include "World.h"
#include "Renderer.h"


World::World()
{
}


World::~World()
{
}

HeightMapGenerator::SurroundingHeightmaps World::GenerateSurroundingHeightmaps(int xChunkValue, int zChunkValue)
{
	HeightMapGenerator::SurroundingHeightmaps toReturn;

	if (zChunkValue != 0)
	{
		toReturn.leftHeightMap = chunkHeightmaps[xChunkValue][zChunkValue - 1];
	}

	if (xChunkValue != (CHUNK_ARRAY_COUNT - 1))
	{
		toReturn.topHeightMap = chunkHeightmaps[xChunkValue + 1][zChunkValue];
	}

	if (zChunkValue != (CHUNK_ARRAY_COUNT - 1))
	{
		toReturn.rightHeightMap = chunkHeightmaps[xChunkValue][zChunkValue + 1];
	}

	if (xChunkValue != 0)
	{
		toReturn.bottomHeightMap = chunkHeightmaps[xChunkValue - 1][zChunkValue];
	}

	return toReturn;
}

void World::GenerateWorldThroughCombination()
{
	//Generate chunks. Left->Right = +Z, Bottom->Top = +X
	for (int cgx = 0; cgx < CHUNK_ARRAY_COUNT; cgx++)
	{
		for (int cgz = 0; cgz < CHUNK_ARRAY_COUNT; cgz++)
		{
			HeightMapGenerator* gen = new HeightMapGenerator(GenerateSurroundingHeightmaps(cgx, cgz));
			gen->GenerateHeightMapUsingCombination(cgx, cgz);
			chunkHeightmaps[cgx][cgz] = gen->GetHeightMapAsArray();
			delete gen;
		}
	}
}

void World::GenerateSingleHeightmapSimplex()
{
	HeightMapGenerator *gen = new HeightMapGenerator(GenerateSurroundingHeightmaps(0, 0));
	gen->GenerateHeightmapSimplex(1025, 300); //512 300
	chunkHeightmaps[0][0] = gen->GetHeightMapAsArray();
	delete gen;
}

void World::GenerateSingleHeightmapDS()
{
	HeightMapGenerator *gen = new HeightMapGenerator(GenerateSurroundingHeightmaps(0, 0));
	gen->GenerateHeightmapSingleDS();
	chunkHeightmaps[0][0] = gen->GetHeightMapAsArray();
	delete gen;
}

void World::GenerateSingleTestCompletelyRandom()
{
	HeightMapGenerator *gen = new HeightMapGenerator(GenerateSurroundingHeightmaps(0, 0));
	gen->GenerateCompleteRandom();
	chunkHeightmaps[0][0] = gen->GetHeightMapAsArray();
	delete gen;
}

void World::GenerateBlankMap()
{
	HeightMapGenerator *gen = new HeightMapGenerator(GenerateSurroundingHeightmaps(0, 0));
	gen->GenerateBlankMap();
	chunkHeightmaps[0][0] = gen->GetHeightMapAsArray();
	delete gen;
}

void World::RenderWorld(Renderer *renderer)
{
	GenerateMeshesFromWorld();

	for (int cgx = 0; cgx < CHUNK_ARRAY_COUNT; cgx++)
	{
		for (int cgz = 0; cgz < CHUNK_ARRAY_COUNT; cgz++)
		{
			renderer->AddToPipeline(chunkMeshes[cgx][cgz]);
			renderer->AddToPipeline(waterLevelMeshes[cgx][cgz]);
		}
	}

}

void World::GenerateMeshesFromWorld()
{
	for (int cgx = 0; cgx < CHUNK_ARRAY_COUNT; cgx++)
	{
		for (int cgz = 0; cgz < CHUNK_ARRAY_COUNT; cgz++)
		{
			chunkMeshes[cgx][cgz] = Mesh::GenerateChunk(chunkHeightmaps[cgx][cgz], cgz, cgx);
			waterLevelMeshes[cgx][cgz] = Mesh::GenerateWaterChunk(cgz, cgx);
		}
	}
}

void World::RenderSingleMesh(Renderer *renderer)
{
	renderer->AddToPipeline(Mesh::GenerateChunkGreyScale(chunkHeightmaps[0][0], 0, 0));
}
