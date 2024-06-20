#include "App.hpp"

#include <stdexcept>
#include <iostream>
#include <crtdbg.h>

int main() {
	App app;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	try { app.Run(); }
	catch (const std::exception& e) {
		std::cout << "Exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}