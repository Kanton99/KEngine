#include "App.h"
#include <iostream>
int Entity::eCounter = 0;
int main() {
	App *app = new App();
	app->start();
	return 0;
}