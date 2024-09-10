#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include "vkEngine.h"

int main() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Failed to init video, Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("test windows",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		680, 480,
		SDL_WindowFlags(SDL_WINDOW_VULKAN));

	if (!window) {
		std::cerr << "Failed to create window, Error: " << SDL_GetError() << std::endl;
		return -1;
	}
	//				
	//SDL_Surface* window_surface = SDL_GetWindowSurface(window);

	//if (!window_surface)
	//{
	//	std::cout << "Failed to get the surface from the window\n";
	//	return -1;
	//}

	vkEngine *engine = new vkEngine(window);
	engine->init();

	engine->cleanup();
	return 0;
}