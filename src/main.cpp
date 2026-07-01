#include "pch.h"

#include "system.h"
#include "graphics.h"
#include "memory.h"
#include "content.h"
#include "swapChain.h"
#include "pipeline.h"

int main(int argc, char* argv[]) {
	tracer::system::initialize();
	tracer::graphics::initialize();
	tracer::graphics::memory::initialize();
	tracer::graphics::content::load();
	tracer::graphics::swapChain::initialize();
	auto pipeline = std::make_unique<tracer::graphics::Pipeline>("vertex", "pixel");

	while (tracer::system::poll()) {
		tracer::graphics::swapChain::begin();
		pipeline->bind();
		tracer::graphics::swapChain::bind();
		tracer::graphics::content::draw();
		tracer::graphics::swapChain::end();
		tracer::graphics::execute();
		tracer::graphics::swapChain::present();
	}

	tracer::graphics::swapChain::destroy();

	return EXIT_SUCCESS;
}
