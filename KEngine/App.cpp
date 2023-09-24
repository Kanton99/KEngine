#include "App.h"

void App::Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		default:
			break;
		}
	}
}

void App::Update()
{
	this->root.update();
}

void App::Render()
{
	
}

void App::start()
{
	/*std::cout << "test" << std::endl;*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("MyWindow", 0, 20, 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	while (true) {
		Input();
		Update();
		Render();
	}
}
