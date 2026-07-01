#include "pch.h"

#include "swapChainImage.h"

#include "system.h"
#include "graphics.h"

#include "verify.h"

namespace tracer::graphics::swapChain {
	struct Image::Implementation {
		Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView;
		D3D12_CPU_DESCRIPTOR_HANDLE constantBufferHostView;
		D3D12_GPU_DESCRIPTOR_HANDLE constantBufferDeviceView;
		uint8_t* constantBufferMemory;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		uint64_t fenceValue;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
		D3D12_RESOURCE_BARRIER renderBarrier;
		D3D12_RESOURCE_BARRIER presentBarrier;
	};

	Image::Image(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, D3D12_CPU_DESCRIPTOR_HANDLE constantBufferHostView, D3D12_GPU_DESCRIPTOR_HANDLE constantBufferDeviceView, uint8_t* constantBufferMemory) : implementation(std::make_unique<Implementation>()) {
		implementation->swapChainBuffer = swapChainBuffer;
		implementation->renderTargetView = renderTargetView;
		implementation->constantBufferHostView = constantBufferHostView;
		implementation->constantBufferDeviceView = constantBufferDeviceView;
		implementation->constantBufferMemory = constantBufferMemory;

		auto device = getDevice();

		VERIFY_COM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(implementation->commandAllocator.GetAddressOf())));
		std::println("\tCommand allocator created");

		implementation->fenceValue = 0;
		VERIFY_COM(device->CreateFence(implementation->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(implementation->fence.GetAddressOf())));
		std::println("\tFence created and value set");
		
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

	void Image::wait() {
		if (implementation->fence->GetCompletedValue() < implementation->fenceValue) {
			auto event = system::getEvent();

			VERIFY_COM(implementation->fence->SetEventOnCompletion(implementation->fenceValue, event));
			VERIFY_COM(WaitForSingleObject(event, INFINITE));
		}
	}

	Image::~Image() = default;
}
