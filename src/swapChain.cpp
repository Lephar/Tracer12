#include "pch.h"

#include "swapChain.h"
#include "swapChainImage.h"

#include "system.h"
#include "graphics.h"
#include "memory.h"

#include "helper.h"

namespace tracer::graphics::swapChain {
	namespace {
		const uint32_t imageCount = 3;

		const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
		const DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_VIEWPORT viewport = {};
		D3D12_RECT scissor = {};

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> depthStencilBuffer = nullptr;

		CD3DX12_RESOURCE_BARRIER renderBarrier = {};
		CD3DX12_RESOURCE_BARRIER presentBarrier = {};

		std::vector<Image> images = {};

		uint32_t imageIndex = UINT32_MAX;
	}

	const uint32_t getImageCount() {
		return imageCount;
	}

	const DXGI_FORMAT getDepthStencilFormat() {
		return depthStencilFormat;
	}

	const DXGI_FORMAT getRenderTargetFormat() {
		return renderTargetFormat;
	}

	void initialize() {
		const auto width = system::getWidth();
		const auto height = system::getHeight();

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

		VERIFY_COM(getFactory()->CreateSwapChainForHwnd(getCommandQueue().Get(), system::getWindow(), &swapChainDesc, &fullscreenDesc, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())));
		std::println("Swap chain created with {} images", imageCount);

		auto device = getDevice();

		auto depthStencilResourceDesc = CD3DX12_RESOURCE_DESC1::Tex2D(depthStencilFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			.Format = depthStencilFormat,
			.DepthStencil = {
				.Depth = 1.0f,
				.Stencil = 0,
			},
		};

		auto defaultHeapProperties = memory::getDefaultHeapProperties();

		VERIFY_COM(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue, nullptr, IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));
		std::println("Depth stencil buffer created on default heap");

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
			.Format = depthStencilFormat,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		};

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, memory::getDepthStencilDescriptorHeap()->GetFirstCpuHandle());
		std::println("Depth stencil view created");

		images.reserve(imageCount);

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			std::println("Swap chain image {}:", imageIndex);

			Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer = nullptr;
			VERIFY_COM(swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf())));
			std::println("\tSwap chain buffer acquired");

			auto renderTargetView = memory::getRenderTargetDescriptorHeap()->GetCpuHandle(imageIndex);
			device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, renderTargetView);
			std::println("\tRender target view created");

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
			VERIFY_COM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
			std::println("\tCommand allocator created");

			Microsoft::WRL::ComPtr<ID3D12Fence1> fence = nullptr;
			VERIFY_COM(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
			std::println("\tFence created");
		
			images.emplace_back(renderTargetBuffer, renderTargetView, commandAllocator, fence);
		}
	}
	
	void resize() {
	}
	
	void destroy() {
	}
}
