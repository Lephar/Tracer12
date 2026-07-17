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
			const auto aspectRatio = static_cast<float>(width) / static_cast<float>(height);

			graphics::initialize(dataFolder, window, width, height);

			const auto device = graphics::getDevice();
			const auto commandList = graphics::getCommandList();

			content::load(dataFolder, aspectRatio);

			const auto constantBufferSize = content::getConstantBufferSize();
			const auto textureCount = static_cast<uint32_t>(content::getTextures().size());

			graphics::createResources(constantBufferSize, textureCount);

			content::createResources(device);

			graphics::beginCommand();
			content::recordUpload(commandList);
			graphics::endCommand();

			content::clearStaging();
		}

		void loop() {
			const auto commandList = graphics::getCommandList();

			system::prepareLoop();

			while (system::poll()) {
				const auto mouseMovement = system::getMouseMovement();
				const auto keyboardMovement = system::getKeyboardMovement();

				content::update(mouseMovement, keyboardMovement);

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
