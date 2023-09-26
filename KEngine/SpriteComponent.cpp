#include "SpriteComponent.h"

SpriteComponent::SpriteComponent():sprite(NULL)
{
}

SpriteComponent::SpriteComponent(const char* path):sprite(IMG_Load(path))
{
	if (!sprite) {
		throw std::exception("image not found");
		exit(-1);
	}
}

void SpriteComponent::assignSprite(const char* path)
{
	sprite = IMG_Load(path);
	if (!sprite) throw std::exception("image not found");
}

void SpriteComponent::render(SDL_Renderer* context)
{
	SDL_Texture *text = SDL_CreateTextureFromSurface(context, sprite);
	int w, h;
	SDL_Rect rect;
	rect.w = sprite->w;
	rect.h = sprite->h;
	if(this->owner)
	SDL_RenderCopy(context, text, NULL, &rect);
}
