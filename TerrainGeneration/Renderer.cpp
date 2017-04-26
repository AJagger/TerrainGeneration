#include "stdafx.h"
#include "Renderer.h"
#include "Renderer/GLEW/include/GL/glew.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {

	//Generate chunks. Left->Right = +Z, Bottom->Top = +X
	for (int cgx = 0; cgx < CHUNK_ARRAY_COUNT; cgx++)
	{
		for (int cgz = 0; cgz < CHUNK_ARRAY_COUNT; cgz++)
		{
			ChunkGenerator* gen = new ChunkGenerator(GenerateSurroundingHeightmaps(cgx, cgz));
			gen->GenerateHeightMap();
			float **tempMap = gen->GetHeightMapAsArray();
			chunkHeightmaps[cgx][cgz] = tempMap;
			//delete gen;

			chunkMeshes[cgx][cgz] = Mesh::GenerateChunk(tempMap, cgz, cgx);
			waterLevelMeshes[cgx][cgz] = Mesh::GenerateWaterChunk(cgz, cgx);
			//delete tempMap;
		}
	}

	camera = new Camera();

	usingDepth = 1;
	usingAlpha = 1;
	blendMode = 0;

	currentShader = new Shader(SHADERDIR"MatrixVertex.glsl", SHADERDIR"basicFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}

	init = true;

	SwitchToOrthographic();
}

Renderer::~Renderer(void) {
	delete chunk1;
	delete chunk2;
	delete camera;
	delete chunkMeshes;
	delete chunkHeightmaps;

	//for (int i = 0; i < CHUNK_ARRAY_COUNT; i++) {
	//	delete[] chunkHeightmaps[i];
	//}
	//delete[] chunkHeightmaps;
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
}

void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
		width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST | GL_BLEND);

	glUseProgram(currentShader->GetProgram());

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float *)& projMatrix);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float *)& viewMatrix);

	int i = 1;
	//Render Chunk Meshes
	Vector3  tempPos = position;
	tempPos.z += (i*500.0f);
	tempPos.x -= (i*100.0f);
	tempPos.y -= (i*100.0f);

	modelMatrix = Matrix4::Translation(tempPos) *
		Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(scale, scale, scale));

	glUniformMatrix4fv(glGetUniformLocation(
		currentShader->GetProgram(), "modelMatrix"), 1, false,
		(float *)& modelMatrix);

	for (int mdx = 0; mdx < CHUNK_ARRAY_COUNT; mdx++)
	{
		for (int mdz = 0; mdz < CHUNK_ARRAY_COUNT; mdz++)
		{
			waterLevelMeshes[mdx][mdz]->Draw();
			chunkMeshes[mdx][mdz]->Draw();
		}
	}

	glUseProgram(0);
	SwapBuffers();
}

void  Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::PrintMap(float** map) {
	for (int y = 0; y < ChunkGenerator::CHUNK_SIZE; y++)
	{
		for (int x = 0; x < ChunkGenerator::CHUNK_SIZE; x++)
		{
			cout << map[y][x] << ", ";
		}
		cout << "\n";
	}
}

void Renderer::ToggleDepth()
{
	usingDepth = !usingDepth;
	usingDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Renderer::ToggleAlphaBlend()
{
	usingAlpha = !usingAlpha;
	usingAlpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Renderer::ToggleBlendMode()
{
	blendMode = (blendMode + 1) % 4;

	switch (blendMode) {
	case (0): glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
	case (1): glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
	case (2): glBlendFunc(GL_ONE, GL_ZERO); break;
	case (3): glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	};

}

ChunkGenerator::SurroundingHeightmaps Renderer::GenerateSurroundingHeightmaps(int xChunkValue, int zChunkValue)
{
	SurroundingHeightmaps toReturn;

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