#include "pch.h"

#include "graphics.h"

#include "compiler.h"
#include "infrastructure.h"
#include "device.h"
#include "queue.h"
#include "swapchain.h"
#include "rootSignature.h"
#include "pipeline.h"

extern "C" {
	__declspec(dllexport) extern const uint32_t D3D12SDKVersion = 619;
	__declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

namespace tracer::graphics {
	namespace {
		std::map<std::pair<bool, bool>, Pipeline> pipelines;
	}

	void initialize(std::filesystem::path dataFolder, HWND window, uint32_t width, uint32_t height) {
		compiler::initialize(dataFolder);

		infrastructure::initialize();

		auto factory = infrastructure::getFactory();
		auto adapter = infrastructure::getAdapter();

		device::initialize(adapter);

		auto device = device::getDevice();

		queue::initialize(device);

		auto queue = queue::getCommandQueue();

		const uint32_t imageCount = 3;
		const uint32_t sampleCount = 4;
		const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
		const DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		swapChain::initialize(window, factory, device, queue, width, height, imageCount, sampleCount, depthStencilFormat, renderTargetFormat);
	}

	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice() {
		return device::getDevice();
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList() {
		return queue::getCommandList();
	}

	uint32_t getImageCount() {
		return swapChain::getImageCount();
	}

	void createResources(uint32_t constantBufferSize, uint32_t textureCount) {
		auto device = device::getDevice();

		swapChain::createResources(device, constantBufferSize);

		const auto sampleCount = swapChain::getSampleCount();
		const auto depthStencilFormat = swapChain::getDepthStencilFormat();
		const auto renderTargetFormat = swapChain::getRenderTargetFormat();

		rootSignature::create(device, textureCount);

		auto rootSignature = rootSignature::getRootSignature();

		auto vertexShader = compiler::loadShader(L"vertex.hlsl", L"vs_6_9", L"main");
		auto pixelShader = compiler::loadShader(L"pixel.hlsl", L"ps_6_9", L"main");

		const std::vector<bool> states{ false, true };

		for (auto blending : states) {
			for (auto culling : states) {
				std::pair<bool, bool> key{ blending, culling };
				Pipeline value{ device, rootSignature, vertexShader, pixelShader, sampleCount, depthStencilFormat, renderTargetFormat, blending, culling };

				pipelines.emplace(std::make_pair(key, std::move(value)));
			}
		}
	}

	void beginCommand() {
		queue::begin();
	}

	void endCommand() {
		queue::end();
		queue::execute();
		queue::signal();
		queue::wait();
	}

	void beginFrame() {
		auto commandList = queue::getCommandList();
		auto fenceEvent = queue::getFenceEvent();

		swapChain::begin(commandList, fenceEvent);
		rootSignature::bind(commandList);
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> getCurrentConstantBuffer() {
		return swapChain::getCurrentConstantBuffer();
	}

	void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, bool blending, bool culling) {
		pipelines.at(std::make_pair(blending, culling)).bind(commandList);
	}

	void endFrame() {
		auto commandQueue = queue::getCommandQueue();
		auto commandList = queue::getCommandList();

		swapChain::end(commandList);
		queue::execute();
		swapChain::present(commandQueue);
	}

	void destroy() {
		auto commandQueue = queue::getCommandQueue();
		auto commandList = queue::getCommandList();
		auto fenceEvent = queue::getFenceEvent();

		swapChain::destroy(commandQueue, commandList, fenceEvent);
	}
}
