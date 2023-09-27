#include "App.h"

void App::Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
			exit(0);
			break;
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Could not initialize! SDL_error: %s\n", SDL_GetError());
		exit(-1);
	}
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
