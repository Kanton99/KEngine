#include "App.h"
#include <iostream>
#include "Coordinator.h"
//int Entity::eCounter = 0;
Coordinator gCoordinator;
int main() {
	try {
		App* app = new App();
		app->init();
		app->start();
		return 0;
	}
	catch (std::exception e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
}