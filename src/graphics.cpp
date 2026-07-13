#include "pch.h"

#include "graphics.h"

#include "compiler.h"
#include "infrastructure.h"
#include "device.h"
#include "queue.h"
#include "memory.h"
#include "swapchain.h"
#include "pipeline.h"

#include "verify.h"

extern "C" {
	__declspec(dllexport) extern const uint32_t D3D12SDKVersion = 619;
	__declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

namespace tracer::graphics {
	namespace {
		std::unique_ptr<Pipeline> pipeline;
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
		const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
		const DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		swapChain::initialize(window, factory, device, queue, width, height, imageCount, depthStencilFormat, renderTargetFormat);
		
		auto vertexShader = compiler::loadShader(L"vertex.hlsl", L"vs_6_9", L"main");
		auto pixelShader = compiler::loadShader(L"pixel.hlsl", L"ps_6_9", L"main");

		pipeline = std::make_unique<Pipeline>(device, vertexShader, pixelShader, depthStencilFormat, renderTargetFormat);
	}

	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice() {
		return device::getDevice();
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList() {
		return queue::getCommandList();
	}

	void createResources(uint32_t materialCount, uint32_t materialTextureCount) {
		auto device = device::getDevice();
		const auto imageCount = swapChain::getImageCount();

		memory::allocate(device, imageCount, materialCount, materialTextureCount);

		const auto defaultHeapProperties = memory::getDefaultHeapProperties();
		auto depthStencilDescriptorHeap = memory::getDepthStencilDescriptorHeap();
		auto renderTargetDescriptorHeap = memory::getRenderTargetDescriptorHeap();

		swapChain::createResources(device, defaultHeapProperties, depthStencilDescriptorHeap, renderTargetDescriptorHeap);
	}

	D3D12_HEAP_PROPERTIES getDefaultHeapProperties() {
		return memory::getDefaultHeapProperties();
	}

	D3D12_HEAP_PROPERTIES getUploadHeapProperties() {
		return memory::getUploadHeapProperties();
	}

	std::shared_ptr<DirectX::DescriptorHeap> getDepthStencilDescriptorHeap() {
		return memory::getDepthStencilDescriptorHeap();
	}

	std::shared_ptr<DirectX::DescriptorHeap> getRenderTargetDescriptorHeap() {
		return memory::getRenderTargetDescriptorHeap();
	}

	std::shared_ptr<DirectX::DescriptorHeap> getShaderResourceDescriptorHeap() {
		return memory::getShaderResourceDescriptorHeap();
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
	/*
	void beginFrame() {
		auto commandList = queue::getCommandList();
		swapChain::begin(commandList);
	}

	void endFrame() {
		auto commandList = queue::getCommandList();
		swapChain::end(commandList);
	}
	*/
}
