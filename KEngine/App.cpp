#include "App.h"
#include <time.h>
#include <Windows.h>
#include <memory>
//#include <vulkan.h>
#include <SDL_vulkan.h>
#include "Coordinator.h"
#include "RenderSystem.h"
//#include "RenderSystem.h"
#include <random>

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

	std::mt19937_64 rng(std::random_device{}());

	// Define the range of the random float
	std::uniform_real_distribution<float> dist(0.0f, 0.01f);
	//for (int i = 0; i < 10; i++){
	//	auto GunAxe = gCoordinator.createEntity();
	//	auto space = glm::mat4(1.f);

	//	// Generate a random float
	//	float random_float = dist(rng);
	//	space[0][3] = random_float;
	//	random_float = dist(rng);
	//	space[2][3] = random_float;
	//	gCoordinator.addComponent(GunAxe, Transform{ space });
	//	gCoordinator.addComponent(GunAxe, RenderComponent());

	//	renderSystem->loadModel(GunAxe, "Resources/Models/gunaxe.obj");
	//	renderSystem->loadTexture(GunAxe, "Resources/Textures/lambert4_Base_color.png");
	//}
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
