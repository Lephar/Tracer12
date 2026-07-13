#include "pch.h"

#include "frameBuffer.h"

#include "system.h"
#include "graphics.h"
#include "memory.h"
#include "content.h"

#include "debug.h"

namespace tracer::graphics {
	struct FrameBuffer::Implementation {
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
		uint64_t fenceValue;

		Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView;

		D3D12_RESOURCE_BARRIER renderBarrier;
		D3D12_RESOURCE_BARRIER presentBarrier;
	};

	FrameBuffer::FrameBuffer(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, Microsoft::WRL::ComPtr<ID3D12Fence1> fence) : implementation(std::make_unique<Implementation>()) {
		implementation->commandAllocator = commandAllocator;
		implementation->fence = fence;
		implementation->fenceValue = 0;
		debug::print("Fence value set");
	}

	FrameBuffer::FrameBuffer(FrameBuffer&& image) noexcept : implementation(std::move(image.implementation)) {}

	FrameBuffer& FrameBuffer::operator=(FrameBuffer&& image) noexcept {
		implementation = std::move(image.implementation);
		return *this;
	}

	void FrameBuffer::setResources(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView) {
		implementation->swapChainBuffer = swapChainBuffer;
		implementation->renderTargetView = renderTargetView;

		implementation->renderBarrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->swapChainBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		debug::print("Render barrier set");

		implementation->presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->swapChainBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		debug::print("Present barrier set");
	}
	/*
	void FrameBuffer::wait() {
		if (implementation->fence->GetCompletedValue() < implementation->fenceValue) {
			auto event = system::getEvent();

			debug::verify::com(implementation->fence->SetEventOnCompletion(implementation->fenceValue, event));
			debug::verify::com(WaitForSingleObject(event, INFINITE));
		}
	}

	void FrameBuffer::begin() {
		auto commandList = getCommandList();
		auto& constants = content::getConstants();

		memcpy(implementation->constantBufferMemory, &constants, sizeof(constants));

		debug::verify::com(implementation->commandAllocator->Reset());
		debug::verify::com(commandList->Reset(implementation->commandAllocator.Get(), nullptr));

		commandList->ResourceBarrier(1, &implementation->renderBarrier);
	}

	void FrameBuffer::bind(D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView) {
		auto commandList = getCommandList();
		auto constantBufferDescriptorHeap = memory::getConstantBufferDescriptorHeap()->Heap();

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		commandList->SetDescriptorHeaps(1, &constantBufferDescriptorHeap);
		commandList->SetGraphicsRootDescriptorTable(0, implementation->constantBufferDeviceView);
		commandList->OMSetRenderTargets(1, &implementation->renderTargetView, false, &depthStencilView);
		commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);
		commandList->ClearRenderTargetView(implementation->renderTargetView, clearColor, 0, nullptr);
	}
	
	void FrameBuffer::end() {
		auto commandList = getCommandList();

		commandList->ResourceBarrier(1, &implementation->presentBarrier);

		debug::verify::com(commandList->Close());
	}

	void FrameBuffer::signal() {
		implementation->fenceValue++;

		debug::verify::com(getCommandQueue()->Signal(implementation->fence.Get(), implementation->fenceValue));
	}
	*/
	FrameBuffer::~FrameBuffer() = default;
}
