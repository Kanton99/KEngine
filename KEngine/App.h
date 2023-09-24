#pragma once
#include <SDL.h>
#undef main
#include <iostream>

#include "Entity.h"
class App
{
//Variables
private:
	Entity root;

//Functions
private:
	void Input();
	void Update();
	void Render();
public:
	void start();
};

