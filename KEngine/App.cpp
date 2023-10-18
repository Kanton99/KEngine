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
			vkDeviceWaitIdle(engine->device);
			engine->cleanup();
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
	engine = std::make_unique<VulkanEngine>();
	engine->init();


	IMG_Init(IMG_INIT_JPG & IMG_INIT_PNG);
	this->root = std::unique_ptr<Entity2D>(new Entity2D("test2DSprite"));
	std::unique_ptr<Entity2D> spriteTest = std::make_unique<Entity2D>("sprite");
	//spriteTest->addComponent(std::move(std::make_unique<SpriteComponent>("./trasferimento.jpg")));
	this->root->addChild(std::move(spriteTest));

}
