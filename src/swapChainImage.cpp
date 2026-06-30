#include "pch.h"

#include "swapChainImage.h"

namespace tracer::graphics::swapChain {
	struct Image::Implementation {
		Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		uint64_t fenceValue;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
		D3D12_RESOURCE_BARRIER renderBarrier;
		D3D12_RESOURCE_BARRIER presentBarrier;
	};

	Image::Image(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, Microsoft::WRL::ComPtr<ID3D12Fence1> fence) : implementation(std::make_unique<Implementation>()) {
		implementation->swapChainBuffer = swapChainBuffer;
		implementation->renderTargetView = renderTargetView;
		implementation->commandAllocator = commandAllocator;
		implementation->fenceValue = 0;
		implementation->fence = fence;

		implementation->renderBarrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->swapChainBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		std::println("\tRender barrier set");

		implementation->presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->swapChainBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		std::println("\tPresent barrier set");
	}

	Image::Image(Image&& image) noexcept : implementation(std::move(image.implementation)) {}

	Image& Image::operator=(Image&& image) noexcept {
		implementation = std::move(image.implementation);
		return *this;
	}

	Image::~Image() = default;
}
