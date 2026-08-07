#include "pch.h"

#include "tracer.h"

#include "system.h"
#include "content.h"
#include "compiler.h"
#include "infrastructure.h"
#include "device.h"
#include "queue.h"
#include "swapchain.h"
#include "rootSignature.h"
#include "pipeline.h"

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

			rootSignature::create();
			pipeline::create();

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
			auto commandQueue = queue::getCommandQueue();
			auto commandList = queue::getCommandList();
			auto fenceEvent = queue::getFenceEvent();

			swapChain::destroy(commandQueue, commandList, fenceEvent);
		}
	}

	void run() {
		initialize();
		loop();
		destroy();
	}
}
