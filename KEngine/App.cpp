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

void App::Render(SDL_Renderer *context)
{
	this->root.render(context);
}

void App::start()
{
	/*std::cout << "test" << std::endl;*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("MyWindow", 0, 20, 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	Entity2D *spriteTest = new Entity2D("sprite");
	this->root.addChild(spriteTest);
	spriteTest->addComponent(new SpriteComponent("./test.bmp"));


	while (true) {
		Input();
		Update();
		Render(renderer);
	}
}
