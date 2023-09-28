#include "SpriteComponent.h"
#include <iostream>

SpriteComponent::SpriteComponent()
{
	type = SPRITE_COMPONENT;
}

SpriteComponent::SpriteComponent(const char* path)
{
	type = SPRITE_COMPONENT;
	assignSprite(path);
}

SpriteComponent::~SpriteComponent()
{
	SDL_FreeSurface(this->sprite.get());
}
void SpriteComponent::assignSprite(const char* path)
{
	int imgFlags = IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
	}
	sprite = std::unique_ptr<SDL_Surface>(IMG_Load(path));
	if (!sprite) {
		printf("Image not loaded: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
}

void SpriteComponent::render(SDL_Renderer* context)
{
	
	SDL_Texture* text = SDL_CreateTextureFromSurface(context,sprite.get());
	
	SDL_Rect rect = SDL_Rect();
	rect.w = sprite->w;
	rect.h = sprite->h;
	SDL_RenderCopy(context, text, NULL, &rect);
	SDL_DestroyTexture(text);
}
