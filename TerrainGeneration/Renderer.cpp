#include "stdafx.h"
#include "Renderer.h"
#include "Renderer/GLEW/include/GL/glew.h"
#include "DataArray.h"
#include "DataArray.cpp"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {

	camera = new Camera();

	usingDepth = 1;
	usingAlpha = 1;
	blendMode = 0;

	currentShader = new Shader(SHADERDIR"MatrixVertex.glsl", SHADERDIR"basicFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}

	init = true;

	SwitchToPerspective();
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Enable outlines
}

Renderer::~Renderer(void) {
	delete camera;
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

	//Cycle through objects in renderPipeline and draw.
	Mesh *drawData = *renderPipeline.TryToGetFirst();
	if (drawData != nullptr)
	{
		modelMatrix = Matrix4::Translation(position) * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) * Matrix4::Scale(Vector3(scale, scale, scale));
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float *)& modelMatrix);
		drawData->Draw();

		while (renderPipeline.IsNext())
		{
			drawData = *renderPipeline.Next();
			if (drawData != nullptr)
			{
				modelMatrix = Matrix4::Translation(position) * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) * Matrix4::Scale(Vector3(scale, scale, scale));
				glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float *)& modelMatrix);
				drawData->Draw();
			}
		}
	}

	glUseProgram(0);
	SwapBuffers();
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

void  Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::AddToPipeline(Mesh * mesh)
{
	Mesh ** toAdd = renderPipeline.CreateNew();
	*toAdd = mesh;
}