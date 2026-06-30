#include "pch.h"

#include "system.h"
#include "graphics.h"

int main(int argc, char* argv[]) {
	tracer::system::initialize();
	tracer::graphics::initialize();

	while (tracer::system::poll());

	return EXIT_SUCCESS;
}
