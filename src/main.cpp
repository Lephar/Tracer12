#include "pch.h"

#include "window.h"
#include "instance.h"

int main(int argc, char* argv[]) {
	window::createWindow();
	instance::initialize();

	while (window::poll());

	return EXIT_SUCCESS;
}
