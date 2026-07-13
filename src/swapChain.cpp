#include "pch.h"

#include "swapChain.h"
#include "frameBuffer.h"

#include "verify.h"

namespace tracer::graphics::swapChain {
	namespace {
		uint32_t width = 0;
		uint32_t height = 0;

		uint32_t imageCount = 0;

		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_UNKNOWN;

		D3D12_VIEWPORT viewport = {};
		D3D12_RECT scissor = {};

		DirectX::SimpleMath::Matrix projection;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> depthStencilBuffer = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = {};

		std::vector<FrameBuffer> frameBuffers = {};

		uint32_t imageIndex = UINT32_MAX;
	}

	void initialize(
		HWND window,
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory,
		Microsoft::WRL::ComPtr<ID3D12Device15> device,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue1> queue,
		uint32_t swapChainWidth,
		uint32_t swapChainHeight,
		uint32_t swapChainImageCount,
		DXGI_FORMAT swapChainDepthStencilFormat,
		DXGI_FORMAT swapChainRenderTargetFormat
	) {
		width = swapChainWidth;
		height = swapChainHeight;
		imageCount = swapChainImageCount;
		depthStencilFormat = swapChainDepthStencilFormat;
		renderTargetFormat = swapChainRenderTargetFormat;
		std::println("Swap chain properties set");

		viewport = {
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(width),
			.Height = static_cast<float>(height),
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f,
		};

		scissor = {
			.left = 0,
			.top = 0,
			.right = static_cast<long>(width),
			.bottom = static_cast<long>(height),
		};

		std::println("Viewport and scissor set");

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
			.Width = width,
			.Height = height,
			.Format = renderTargetFormat,
			.SampleDesc = {
				.Count = 1,
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = imageCount,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {
			.Windowed = true,
		};

		VERIFY_COM(factory->CreateSwapChainForHwnd(queue.Get(), window, &swapChainDesc, &fullscreenDesc, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())));
		std::println("Swap chain created with {} images", imageCount);
		
		frameBuffers.reserve(imageCount);

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			std::println("Swap chain image {}:", imageIndex);

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			VERIFY_COM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
			std::println("\tCommand allocator created");

			Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
			VERIFY_COM(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
			std::println("\tFence created");

			frameBuffers.emplace_back(commandAllocator, fence);
		}
	}

	uint32_t getImageCount() {
		return imageCount;
	}

	DXGI_FORMAT getDepthStencilFormat() {
		return depthStencilFormat;
	}

	DXGI_FORMAT getRenderTargetFormat() {
		return renderTargetFormat;
	}

	void createResources(
		Microsoft::WRL::ComPtr<ID3D12Device15> device,
		D3D12_HEAP_PROPERTIES defaultHeapProperties,
		std::shared_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap,
		std::shared_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap
	) {
		auto depthStencilResourceDesc = CD3DX12_RESOURCE_DESC1::Tex2D(depthStencilFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			.Format = depthStencilFormat,
			.DepthStencil = {
				.Depth = 0.0f,
				.Stencil = 0,
			},
		};

		VERIFY_COM(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue, nullptr, IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));
		std::println("Depth stencil buffer created on default heap");

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
			.Format = depthStencilFormat,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		};

		depthStencilView = depthStencilDescriptorHeap->GetFirstCpuHandle();
		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView);
		std::println("Depth stencil view created");

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			auto& frameBuffer = frameBuffers.at(imageIndex);
			std::println("Swap chain image {}:", imageIndex);

			Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer;
			VERIFY_COM(swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf())));
			std::println("\tSwap chain buffer acquired");

			auto renderTargetView = renderTargetDescriptorHeap->GetCpuHandle(imageIndex);
			device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, renderTargetView);
			std::println("\tRender target view created");

			frameBuffer.setResources(renderTargetBuffer, renderTargetView);
		}
	}
	/*
	void begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		imageIndex = swapChain->GetCurrentBackBufferIndex();
		auto& image = frameBuffers.at(imageIndex);

		image.wait();
		image.begin();
	}

	void end(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		frameBuffers.at(imageIndex).end();
	}

	void bind() {
		auto commandList = getCommandList();

		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);

		frameBuffers.at(imageIndex).bind(depthStencilView);
	}

	void present() {
		frameBuffers.at(imageIndex).signal();

		DXGI_PRESENT_PARAMETERS presentParameters = {};
		VERIFY_COM(swapChain->Present1(0, 0, &presentParameters));
	}

	void destroy() {
		for (auto& image : frameBuffers) {
			image.signal();
		}
		for (auto& image : frameBuffers) {
			image.wait();
		}
	}
	*/
}
