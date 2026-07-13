#include "pch.h"

#include "tracer.h"

#include "system.h"
#include "graphics.h"
#include "content.h"

#include "debug.h"

namespace tracer {
	namespace {
		void initialize() {
			debug::resetDepth();

			system::initialize("Tracer");
			
			const auto dataFolder = system::getDataFolder();
			const auto window = system::getWindow();
			const auto width = system::getWidth();
			const auto height = system::getHeight();

			graphics::initialize(dataFolder, window, width, height);

			const auto device = graphics::getDevice();
			const auto commandList = graphics::getCommandList();

			content::load(dataFolder);

			const auto materialCount = static_cast<uint32_t>(content::getMaterials().size());
			const auto materialTextureCount = content::getMaterialTextureCount();

			graphics::createResources(materialCount, materialTextureCount);

			const auto defaultHeapProperties = graphics::getDefaultHeapProperties();
			const auto uploadHeapProperties = graphics::getUploadHeapProperties();
			const auto shaderResourceDescriptorHeap = graphics::getShaderResourceDescriptorHeap();

			content::createResources(device, defaultHeapProperties, uploadHeapProperties, shaderResourceDescriptorHeap);

			graphics::beginCommand();
			content::recordUpload(commandList);
			graphics::endCommand();
		}
		/*
		void loop() {
			tracer::graphics::prepareLoop();

			while (system::poll()) {
				graphics::content::update();
				graphics::swapChain::begin();
				pipeline->bind();
				graphics::swapChain::bind();
				graphics::content::draw();
				graphics::swapChain::end();
				graphics::execute();
				graphics::swapChain::present();
			}
		}

		void destroy() {
			graphics::swapChain::destroy();
		}
		*/
	}

	void run() {
		initialize();
		//loop();
		//destroy();
	}
}
