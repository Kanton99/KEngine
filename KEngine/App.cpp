#include "App.h"
#include "SpriteComponent.h"
#include <time.h>
#include <Windows.h>
#include <memory>
//#include <vulkan.h>
#include <SDL_vulkan.h>

#define FRAMERATE 30

void App::Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
			destroy();
			exit(0);
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_RESIZED:
				engine->framebufferResized = true;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void App::Update()
{
	this->root->update();
}

void App::Render(std::shared_ptr<SDL_Renderer> context)
{
	SDL_RenderClear(context.get());
	this->root->render(context);
	SDL_RenderPresent(context.get());
}

void App::start()
{
	engine->loadModel("Resources/Models/gunaxe.obj");
	engine->createTextureImage("Resources/Textures/lambert4_Base_color.png");

	engine->loadModel("Resources/Models/gunaxe.obj");
	engine->createTextureImage("Resources/Textures/lambert4_Base_color.png");
	engine->models[1]->transform = glm::translate(engine->models[1]->transform, glm::vec3(0.1f, 0.f, 0.f));
	float frameLength = 1.f / FRAMERATE;
	while (true) {
		Input();
		/*Update();
		Render(renderer);*/
		engine->drawFrame();
		Sleep(1000 * frameLength);
	}
}

void App::init() {
	//MemoryManager.startUp() //TODO Make Memorymanager
	//ResourceManager.startUp()) //TODO Make resource manager
	engine = VulkanEngine::startUp();
}

void App::destroy() {
	engine->cleanup();
}
