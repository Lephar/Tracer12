#include "pch.h"

#include "tracer.h"

#include "system.h"
#include "graphics.h"
#include "content.h"

namespace tracer {
	namespace {
		void initialize() {
			system::initialize("Tracer");
			
			const auto dataFolder = system::getDataFolder();
			const auto window = system::getWindow();
			const auto width = system::getWidth();
			const auto height = system::getHeight();

			graphics::initialize(dataFolder, window, width, height);

			const auto device = graphics::getDevice();
			const auto commandList = graphics::getCommandList();

			content::load(dataFolder);

			const auto constantBufferSize = content::getConstantBufferSize();
			const auto textureCount = static_cast<uint32_t>(content::getTextures().size());

			graphics::createResources(constantBufferSize, textureCount);

			content::createResources(device);

			graphics::beginCommand();
			content::recordUpload(commandList);
			graphics::endCommand();
		}

		void loop() {
			const auto commandList = graphics::getCommandList();

			while (system::poll()) {
				graphics::beginFrame();
				const auto constantBuffer = graphics::getCurrentConstantBuffer();
				
				content::draw(commandList, constantBuffer);
				graphics::endFrame();
			}
		}

		void destroy() {
			graphics::destroy();
		}
	}

	void run() {
		initialize();
		loop();
		destroy();
	}
}
