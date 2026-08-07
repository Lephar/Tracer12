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
		std::map<std::pair<bool, bool>, Pipeline> pipelines;
		
		void initialize() {
			system::initialize();

			const auto dataFolder = system::getDataFolder();
			const auto window = system::getWindow();
			const auto width = system::getWidth();
			const auto height = system::getHeight();
			const auto aspectRatio = static_cast<float>(width) / static_cast<float>(height);

			compiler::initialize(dataFolder);

			infrastructure::initialize();

			const auto factory = infrastructure::getFactory();
			const auto adapter = infrastructure::getAdapter();

			device::initialize(adapter);

			const auto device = device::getDevice();

			queue::initialize(device);

			const auto queue = queue::getCommandQueue();
			const auto commandList = queue::getCommandList();

			swapChain::initialize(window, factory, device, queue, width, height);

			const auto sampleCount = swapChain::getSampleCount();
			const auto depthStencilFormat = swapChain::getDepthStencilFormat();
			const auto renderTargetFormat = swapChain::getRenderTargetFormat();

			content::load(dataFolder, aspectRatio);

			const auto constantBufferSize = content::getConstantBufferSize();
			const auto textureCount = static_cast<uint32_t>(content::getTextures().size());

			swapChain::createResources(device, constantBufferSize);
			content::createResources(device);

			rootSignature::create(device, textureCount);

			const auto rootSignature = rootSignature::getRootSignature();

			const auto vertexShader = compiler::loadShader(L"vertex.hlsl", L"vs_6_9", L"main");
			const auto pixelShader = compiler::loadShader(L"pixel.hlsl", L"ps_6_9", L"main");

			const std::vector<bool> states{ false, true };

			for (const auto blending : states) {
				for (const auto culling : states) {
					std::pair<bool, bool> key{ blending, culling };
					Pipeline value{ device, rootSignature, vertexShader, pixelShader, sampleCount, depthStencilFormat, renderTargetFormat, blending, culling };

					pipelines.emplace(std::make_pair(key, std::move(value)));
				}
			}

			queue::begin();
			content::recordUpload(commandList);
			queue::end();
			queue::execute();
			queue::signal();
			queue::wait();

			content::clearStaging();
		}

		void loop() {
			const auto commandQueue = queue::getCommandQueue();
			const auto commandList = queue::getCommandList();
			const auto fenceEvent = queue::getFenceEvent();

			system::prepareLoop();

			while (system::poll()) {
				const auto mouseMovement = system::getMouseMovement();
				const auto keyboardMovement = system::getKeyboardMovement();

				content::update(mouseMovement, keyboardMovement);

				swapChain::begin(commandList, fenceEvent);
				rootSignature::bind(commandList);

				const auto constantBuffer = swapChain::getCurrentConstantBuffer();

				content::bind(commandList, constantBuffer);

				const std::vector<bool> states{ false, true };

				for (const auto blending : states) {
					for (const auto culling : states) {
						pipelines.at(std::make_pair(blending, culling)).bind(commandList);
						content::draw(commandList, blending, culling);
					}
				}

				swapChain::end(commandList);
				queue::execute();
				swapChain::present(commandQueue);
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
