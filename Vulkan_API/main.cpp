#ifdef _DEBUG

#include <iostream>
#include "Engine.h"
#include <Windows.h>
int main() {
	Engine *engine = new Engine();
	engine->init();
	while (1) {
		engine->draw();

		Sleep(1000*(1.f/60.f));
	}
	return 0;
}

#endif