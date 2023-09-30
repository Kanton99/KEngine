#include "App.h"
#include "SpriteComponent.h"
#include <time.h>
#include <Windows.h>
#include <memory>
//#include <vulkan.h>
#include <SDL_vulkan.h>

#define FRAMERATE 24

void App::Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
			engine->cleanup();
			exit(0);
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
	//Create window and rendering context
	/*if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Could not initialize! SDL_error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
	SDL_Window* window = SDL_CreateWindow("MyWindow", 50, 50, 800, 600, SDL_WINDOW_VULKAN);
	if (!window) {
		printf("SDL Could not create window: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
	auto renderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);
	if (!renderer.get()) {
		printf("Error: rendered not created: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}*/

	engine = std::make_unique<VulkanEngine>();
	engine->init();
	

	IMG_Init(IMG_INIT_JPG&IMG_INIT_PNG);
	this->root = std::unique_ptr<Entity2D>(new Entity2D("test2DSprite"));
	std::unique_ptr<Entity2D> spriteTest = std::make_unique<Entity2D>("sprite");
	spriteTest->addComponent(std::move(std::make_unique<SpriteComponent>("./trasferimento.jpg")));
	this->root->addChild(std::move(spriteTest));

	float frameLength = 1.f / FRAMERATE;
	while (true) {
		Input();
		/*Update();
		Render(renderer);*/
		Sleep(1000 * frameLength);
	}
}
