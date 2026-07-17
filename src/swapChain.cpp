#include "pch.h"

#include "swapChain.h"
#include "frameBuffer.h"

#include "debug.h"

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

		std::unique_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap = nullptr;
		std::unique_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap = nullptr;

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
		debug::print("Initializing swap chain:");
		debug::incrementDepth();

		width = swapChainWidth;
		height = swapChainHeight;
		imageCount = swapChainImageCount;
		depthStencilFormat = swapChainDepthStencilFormat;
		renderTargetFormat = swapChainRenderTargetFormat;
		debug::print("Swap chain properties set");

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

		debug::print("Viewport and scissor set");

		depthStencilDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
		debug::print("Depth stencil descriptor heap created with size 1");

		renderTargetDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, imageCount);
		debug::print("Render target descriptor heap created with size %u", imageCount);

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

		debug::verify::com(factory->CreateSwapChainForHwnd(queue.Get(), window, &swapChainDesc, &fullscreenDesc, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())));
		debug::print("Swap chain created with %u images", imageCount);
		
		frameBuffers.reserve(imageCount);
		
		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			debug::print("Frame buffer %u:", imageIndex);
			debug::incrementDepth();

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			debug::verify::com(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
			debug::print("Command allocator created");

			Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
			debug::verify::com(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
			debug::print("Fence created");

			frameBuffers.emplace_back(commandAllocator, fence);
			debug::decrementDepth();
		}

		debug::decrementDepth();
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

	void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t constantBufferSize) {
		debug::print("Creating swap chain resources:");
		debug::incrementDepth();

		const auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto depthStencilResourceDesc = CD3DX12_RESOURCE_DESC1::Tex2D(depthStencilFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			.Format = depthStencilFormat,
			.DepthStencil = {
				.Depth = 0.0f,
				.Stencil = 0,
			},
		};

		debug::verify::com(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue, nullptr, IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));
		debug::print("Depth stencil buffer created on default heap");

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
			.Format = depthStencilFormat,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		};

		depthStencilView = depthStencilDescriptorHeap->GetFirstCpuHandle();
		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView);
		debug::print("Depth stencil view created");

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			debug::print("Frame buffer %u:", imageIndex);
			debug::incrementDepth();

			auto& frameBuffer = frameBuffers.at(imageIndex);

			Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer;
			debug::verify::com(swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf())));
			debug::print("Swap chain buffer acquired");

			auto renderTargetView = renderTargetDescriptorHeap->GetCpuHandle(imageIndex);
			device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, renderTargetView);
			debug::print("Render target view created");

			const auto constantBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(constantBufferSize);
			Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer;
			debug::verify::com(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &constantBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(constantBuffer.GetAddressOf())));
			debug::print("Constant buffer created on upload heap");

			frameBuffer.setResources(renderTargetBuffer, renderTargetView, constantBuffer);
			debug::decrementDepth();
		}

		debug::decrementDepth();
	}
	
	void begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, HANDLE fenceEvent) {
		imageIndex = swapChain->GetCurrentBackBufferIndex();
		auto& frameBuffer = frameBuffers.at(imageIndex);

		frameBuffer.wait(commandList, fenceEvent);
		frameBuffer.begin(commandList);
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> getCurrentConstantBuffer() {
		return frameBuffers.at(imageIndex).getConstantBuffer();
	}

	void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);

		frameBuffers.at(imageIndex).bind(commandList, depthStencilView);
	}

	void end(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		frameBuffers.at(imageIndex).end(commandList);
	}

	void present(Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue) {
		frameBuffers.at(imageIndex).signal(commandQueue);

		DXGI_PRESENT_PARAMETERS presentParameters = {};
		debug::verify::com(swapChain->Present1(0, 0, &presentParameters));
	}

	void destroy(Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, HANDLE fenceEvent) {
		for (auto& image : frameBuffers) {
			image.signal(commandQueue);
		}
		for (auto& image : frameBuffers) {
			image.wait(commandList, fenceEvent);
		}
	}
}
