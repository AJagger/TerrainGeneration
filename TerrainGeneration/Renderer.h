#pragma once
#include "Renderer/GLEW/include/GL/glew.h"
#include "Renderer/GLEW/include/GL/wglew.h"

#include "Renderer/nclgl/OGLRenderer.h"
#include "Renderer/nclgl/Camera.h"
#include "DataArray.h"


class  Renderer : public  OGLRenderer {

public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void SwitchToPerspective();
	void SwitchToOrthographic();

	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void AddToPipeline(Mesh *mesh);

	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }

	DataArray<Mesh*> renderPipeline = DataArray<Mesh*>();

protected:
	Camera* camera;

	bool usingDepth;
	bool usingAlpha;
	int blendMode;

	float scale = 1;
	float rotation = 0;
	Vector3 position = Vector3(0,0,0);
};

