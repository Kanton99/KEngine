#include "App.h"
#include <time.h>
#include <Windows.h>
#include <memory>
//#include <vulkan.h>
#include <SDL_vulkan.h>
#include "Coordinator.h"
//#include "RenderSystem.h"

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
	//this->root->update();
}

void App::Render()
{
	renderSystem->draw();
}

void App::start()
{
	/*engine->loadModel("Resources/Models/gunaxe.obj");
	engine->createTextureImage("Resources/Textures/lambert4_Base_color.png");

	engine->loadModel("Resources/Models/gunaxe.obj");
	engine->createTextureImage("Resources/Textures/lambert4_Base_color.png");
	engine->models[1]->transform = glm::translate(engine->models[1]->transform, glm::vec3(0.1f, 0.f, 0.f));*/
	gCoordinator.Init();
	gCoordinator.registerComponent<RenderComponent>();
	gCoordinator.registerComponent<Transform>();

	renderSystem = gCoordinator.registerSystem<RenderSystem>();

	Signature signature;
	signature.set(gCoordinator.getComponentType<Transform>());
	signature.set(gCoordinator.getComponentType<RenderComponent>());
	gCoordinator.setSystemSignature<RenderSystem>(signature);

	auto GunAxe1 = gCoordinator.createEntity();

	gCoordinator.addComponent(GunAxe1, Transform{glm::mat4()});
	gCoordinator.addComponent(GunAxe1, RenderComponent());

	renderSystem->loadModel(GunAxe1, "Resources/Models/gunaxe.obj");

	float frameLength = 1.f / FRAMERATE;
	while (true) {
		Input();
		/*Update();*/
		
		Render();
		Sleep(1000 * frameLength);
	}
}

void App::init() {
	//MemoryManager.startUp() //TODO Make Memorymanager
	//ResourceManager.startUp()) //TODO Make resource manager
	//engine = VulkanEngine::startUp();
	gCoordinator.Init();
}

void App::destroy() {
	//engine->cleanup();
	renderSystem->cleanUp();
}
