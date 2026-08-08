#include "pch.h"
#include "tracer.h"
#include "system.h"
#include "compiler.h"
#include "infrastructure.h"
#include "device.h"
#include "queue.h"
#include "swapchain.h"
#include "memory.h"
#include "rootSignature.h"
#include "pipeline.h"
#include "content.h"

namespace tracer {
	namespace {
		void initialize() {
			system::initialize();
			compiler::initialize();
			infrastructure::initialize();
			device::initialize();
			queue::initialize();
			swapChain::initialize();

			content::load();

			//memory::allocate();
			rootSignature::create();
			pipeline::build();

			swapChain::createResources();
			content::createResources();

			queue::begin();
			content::recordUpload();
			queue::end();
			queue::execute();
			queue::signal();
			queue::wait();

			content::clearStaging();
		}

		void loop() {
			system::prepareLoop();

			while (system::poll()) {
				content::update();

				swapChain::begin();
				content::bind();
				content::draw();
				swapChain::end();

				queue::execute();
				swapChain::present();
			}
		}

		void destroy() {
			swapChain::destroy();
		}
	}

	void run() {
		initialize();
		loop();
		destroy();
	}
}
