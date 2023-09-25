#include "App.h"
#include "Math.h"
#include <iostream>
int main() {
	/*App *app = new App();
	app->start();*/
	Matrix3x3 test(1, 2, 3, 4, 5, 6, 7, 8, 0);
	std::cout << test.toString() << std::endl;
	std::cout << test.deteminant() << std::endl;
	auto inv = test.inverse();
	std::cout << inv.toString() << std::endl;

	std::cout << (test * inv).toString() << std::endl;
	return 0;
}