#include "pch.h"

#include "window.h"
#include "instance.h"
#include "swapChain.h"

int main(int argc, char* argv[]) {
	window::createWindow();
	instance::initialize();
	swapChain::initialize();

	while (window::poll());

	return EXIT_SUCCESS;
}
