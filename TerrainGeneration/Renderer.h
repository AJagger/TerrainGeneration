#pragma once
#include "Renderer/GLEW/include/GL/glew.h"
#include "Renderer/GLEW/include/GL/wglew.h"

#include "Renderer/nclgl/OGLRenderer.h"
#include "Renderer/nclgl/Camera.h"


class  Renderer : public  OGLRenderer {

public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void SwitchToPerspective();
	void SwitchToOrthographic();
	void PrintMap(float **map);

	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();

	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }

	typedef ChunkGenerator::SurroundingHeightmaps SurroundingHeightmaps;
	static const int CHUNK_ARRAY_COUNT = 4;

	SurroundingHeightmaps GenerateSurroundingHeightmaps(int xChunkValue, int zChunkValue);

	float** chunkHeightmaps[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};

protected:
	Mesh* chunkMeshes[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};
	Mesh* waterLevelMeshes[CHUNK_ARRAY_COUNT][CHUNK_ARRAY_COUNT] = {};
	Mesh* chunk1;
	Mesh* chunk2;
	Camera* camera;

	bool usingDepth;
	bool usingAlpha;
	int blendMode;

	float scale;
	float rotation;
	Vector3 position;
	//Vector3 positions[2];
};

