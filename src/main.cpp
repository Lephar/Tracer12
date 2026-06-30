#include "pch.h"

#include "system.h"
#include "graphics.h"
#include "swapChain.h"

int main(int argc, char* argv[]) {
	tracer::system::initialize();
	tracer::graphics::initialize();
	tracer::graphics::swapChain::initialize();

	while (tracer::system::poll());

	return EXIT_SUCCESS;
}
