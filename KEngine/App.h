#pragma once
#include <SDL.h>
#undef main
#include <iostream>

#include "Entity.h"
#include "Entity2D.h"

#include "Components.h"
class App
{
//Variables
private:
	Entity root;

//Functions
private:
	void Input();
	void Update();
	void Render(SDL_Renderer* context);
public:
	void start();
};

