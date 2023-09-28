#include "App.h"
#include "SpriteComponent.h"
#include <time.h>
#include <Windows.h>
#include <memory>

#define FRAMERATE 24

void App::Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
			SDL_Quit();
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

void App::Render(SDL_Renderer *context)
{
	SDL_RenderClear(context);
	this->root->render(context);
	SDL_RenderPresent(context);
}

void App::start()
{
	/*std::cout << "test" << std::endl;*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Could not initialize! SDL_error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
	SDL_Window* window = SDL_CreateWindow("MyWindow", 0, 20, 800, 600, 0);
	if (!window) {
		printf("SDL Could not create window: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		printf("Error: rendered not created: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	IMG_Init(IMG_INIT_JPG);
	this->root = std::unique_ptr<Entity2D>(new Entity2D("test2DSprite"));
	std::unique_ptr<Entity2D> spriteTest = std::make_unique<Entity2D>("sprite");
	spriteTest->addComponent(std::move(std::make_unique<SpriteComponent>("./trasferimento.jpg")));
	this->root->addChild(std::move(spriteTest));

	float frameLength = 1.f / FRAMERATE;
	while (true) {
		Input();
		Update();
		Render(renderer);
		/*SDL_RenderClear(renderer);
		SDL_Surface* surf = IMG_Load("./trasferimento.jpg");
		SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
		SDL_RenderCopy(renderer, text, NULL, NULL);
		SDL_RenderPresent(renderer);*/
		Sleep(1000 * frameLength);
	}
}
