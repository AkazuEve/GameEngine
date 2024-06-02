#include "App.hpp"

#include "stdexcept"

int main() {

	App app;

	try { app.Run(); }
	catch (const std::exception& e) {
		std::cout << "Exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}