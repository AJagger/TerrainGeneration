// TerrainGeneration.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Renderer/nclgl/Window.h"
#include "Renderer.h"
#include "World.h"


int main()
{
	Window w("Terrain Generation!", 1600, 1200, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	float scale = 0.5f;
	float rotation = 90.0f;
	Vector3 position(0, -600.0f, -1500.0f);

	World *world = new World();
	world->GenerateWorldThroughCombination();
	world->RenderWorld(&renderer);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
			renderer.SwitchToOrthographic();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2))
			renderer.SwitchToPerspective();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))
			++scale;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS))
			--scale;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
			++rotation;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
			--rotation;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
			position.y -= 1.0f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
			position.y += 1.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
			position.x -= 1.0f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
			position.x += 1.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
			position.z -= 1.0f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))
			position.z += 1.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3))
			renderer.ToggleAlphaBlend();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_4))
			renderer.ToggleDepth();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_5))
			renderer.ToggleBlendMode();

		renderer.SetRotation(rotation);
		renderer.SetScale(scale);
		renderer.SetPosition(position);

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	delete world;

	return 0;
}

