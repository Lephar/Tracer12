#include "pch.h"

#include "swapChainImage.h"

#include "system.h"
#include "graphics.h"
#include "memory.h"
#include "content.h"

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

	void Image::begin() {
		auto commandList = getCommandList();
		auto& constants = content::getConstants();

		memcpy(implementation->constantBufferMemory, &constants, sizeof(constants));

		VERIFY_COM(implementation->commandAllocator->Reset());
		VERIFY_COM(commandList->Reset(implementation->commandAllocator.Get(), nullptr));

		commandList->ResourceBarrier(1, &implementation->renderBarrier);
	}

	void Image::bind(D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView) {
		auto commandList = getCommandList();
		auto constantBufferDescriptorHeap = memory::getConstantBufferDescriptorHeap()->Heap();

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		commandList->SetDescriptorHeaps(1, &constantBufferDescriptorHeap);
		commandList->SetGraphicsRootDescriptorTable(0, implementation->constantBufferDeviceView);
		commandList->OMSetRenderTargets(1, &implementation->renderTargetView, false, &depthStencilView);
		commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);
		commandList->ClearRenderTargetView(implementation->renderTargetView, clearColor, 0, nullptr);
	}
	
	void Image::end() {
		auto commandList = getCommandList();

		commandList->ResourceBarrier(1, &implementation->presentBarrier);

		VERIFY_COM(commandList->Close());
	}

	void Image::signal() {
		implementation->fenceValue++;

		VERIFY_COM(getCommandQueue()->Signal(implementation->fence.Get(), implementation->fenceValue));
	}

	Image::~Image() = default;
}
