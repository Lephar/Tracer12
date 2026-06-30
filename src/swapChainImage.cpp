#include "pch.h"

#include "swapChainImage.h"

namespace tracer::graphics::swapChain {
	struct Image::Implementation {
		Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer;
		CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetView;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		uint64_t fenceValue;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
	};

	Image::Image(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, Microsoft::WRL::ComPtr<ID3D12Fence1> fence) : implementation(std::make_unique<Implementation>()) {
		implementation->swapChainBuffer = swapChainBuffer;
		implementation->renderTargetView = renderTargetView;
		implementation->commandAllocator = commandAllocator;
		implementation->fenceValue = 0;
		implementation->fence = fence;
	}

	Image::Image(Image&& image) noexcept : implementation(std::move(image.implementation)) {}

	Image& Image::operator=(Image&& image) noexcept {
		implementation = std::move(image.implementation);
		return *this;
	}

	Image::~Image() = default;
}
