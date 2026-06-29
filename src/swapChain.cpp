#include "pch.h"

#include "swapChain.h"

#include "window.h"
#include "instance.h"
//#include "renderTarget.h"

#include "helper.h"

namespace swapChain {
	namespace {
		const uint32_t imageCount = 3;

		const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
		const DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_VIEWPORT viewport = {};
		D3D12_RECT scissor = {};

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthStencilDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetDescriptorHeap = nullptr;

		CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilView = {};
		CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewBegin = {};

		uint32_t renderTargetDescriptorSize = 0;

		Microsoft::WRL::ComPtr<ID3D12Resource2> depthStencilBuffer = nullptr;

		//std::vector<RenderTarget> renderTargets = {};

		uint32_t imageIndex = UINT32_MAX;
	}

	void initialize() {
		auto width = window::getWidth();
		auto height = window::getHeight();

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

		VERIFY_COM(instance::getFactory()->CreateSwapChainForHwnd(instance::getCommandQueue().Get(), window::getWindow(), &swapChainDesc, &fullscreenDesc, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())));
		std::println("Swap chain created");

		auto device = instance::getDevice();

		D3D12_DESCRIPTOR_HEAP_DESC depthStencilDescriptorHeapDesc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		};

		VERIFY_COM(device->CreateDescriptorHeap(&depthStencilDescriptorHeapDesc, IID_PPV_ARGS(depthStencilDescriptorHeap.GetAddressOf())));
		std::println("Depth stencil descriptor heap created");

		D3D12_DESCRIPTOR_HEAP_DESC renderTargetDescriptorHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = imageCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		};

		VERIFY_COM(device->CreateDescriptorHeap(&renderTargetDescriptorHeapDesc, IID_PPV_ARGS(renderTargetDescriptorHeap.GetAddressOf())));
		std::println("Render target descriptor heap created");

		depthStencilView = depthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		std::println("Depth stencil descriptor heap handle acquired");

		renderTargetViewBegin = renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		std::println("Render target descriptor heap handle acquired");

		renderTargetDescriptorSize = device->GetDescriptorHandleIncrementSize(renderTargetDescriptorHeapDesc.Type);
		std::println("Render target descriptor handle increment size acquired");

		auto depthStencilResourceDesc = CD3DX12_RESOURCE_DESC1::Tex2D(depthStencilFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			.Format = depthStencilFormat,
			.DepthStencil = {
				.Depth = 1.0f,
				.Stencil = 0,
			},
		};

		auto defaultHeapProperties = instance::getDefaultHeapProperties();
		VERIFY_COM(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue, nullptr, IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));
		std::println("Depth stencil buffer created on default heap");

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
			.Format = depthStencilFormat,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		};

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView);
		std::println("Depth stencil view created");

		//renderTargets.reserve(imageCount);

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			//renderTargets.emplace_back(imageIndex);
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

	D3D12_VIEWPORT getViewport() {
		return viewport;
	}

	D3D12_RECT getScissor() {
		return scissor;
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain4> getSwapChain() {
		return swapChain;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() {
		return depthStencilView;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE getRenderTargetViewBegin() {
		return renderTargetViewBegin;
	}

	uint32_t getRenderTargetDescriptorSize() {
		return renderTargetDescriptorSize;
	}
	/*
	void begin() {
		imageIndex = swapChain->GetCurrentBackBufferIndex();

		auto& renderTarget = renderTargets.at(imageIndex);

		renderTarget.wait();
		renderTarget.begin();
	}

	void bind() {
		auto& renderTarget = renderTargets.at(imageIndex);

		renderTarget.bind();
	}

	void end() {
		auto& renderTarget = renderTargets.at(imageIndex);

		renderTarget.end();
	}

	void present() {
		auto& renderTarget = renderTargets.at(imageIndex);

		renderTarget.signal();

		DXGI_PRESENT_PARAMETERS presentParameters = {};
		VERIFY_COM(swapChain->Present1(0, 0, &presentParameters));
	}

	void destroy() {
		for (auto& renderTarget : renderTargets) {
			renderTarget.wait();
		}
	}*/
}
