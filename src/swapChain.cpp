#include "pch.h"

#include "swapChain.h"

#include "system.h"
#include "graphics.h"

#include "helper.h"

namespace tracer::graphics::swapChain {
	namespace {
		const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
		const DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_VIEWPORT viewport = {};
		D3D12_RECT scissor = {};

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> depthStencilBuffer = nullptr;

		uint32_t imageIndex = UINT32_MAX;
	}

	void initialize() {
		const auto width = system::getWidth();
		const auto height = system::getHeight();
		const auto imageCount = graphics::getImageCount();

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

		VERIFY_COM(graphics::getFactory()->CreateSwapChainForHwnd(graphics::getCommandQueue().Get(), system::getWindow(), &swapChainDesc, &fullscreenDesc, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())));
		std::println("Swap chain created");

		auto device = graphics::getDevice();

		auto depthStencilResourceDesc = CD3DX12_RESOURCE_DESC1::Tex2D(depthStencilFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			.Format = depthStencilFormat,
			.DepthStencil = {
				.Depth = 1.0f,
				.Stencil = 0,
			},
		};

		auto defaultHeapProperties = graphics::getDefaultHeapProperties();

		VERIFY_COM(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue, nullptr, IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));
		std::println("Depth stencil buffer created on default heap");

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
			.Format = depthStencilFormat,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		};

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, graphics::getDepthStencilDescriptorHeap()->GetFirstCpuHandle());
		std::println("Depth stencil view created");

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer = nullptr;
			VERIFY_COM(swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf())));
			std::println("Swap chain image buffer {} acquired", imageIndex);

			auto renderTargetView = graphics::getRenderTargetDescriptorHeap()->GetCpuHandle(imageIndex);
			device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, renderTargetView);
			std::println("Render target view created for swap chain image {}", imageIndex);
		}
	}

	DXGI_FORMAT getDepthStencilFormat() {
		return depthStencilFormat;
	}

	DXGI_FORMAT getRenderTargetFormat() {
		return renderTargetFormat;
	}
	
	void destroy() {
	}
}
