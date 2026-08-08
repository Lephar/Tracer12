#include "pch.h"
#include "swapChain.h"
#include "frameBuffer.h"
#include "device.h"
#include "queue.h"
#include "debug.h"

namespace tracer {
	struct FrameBuffer::Implementation {
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
		uint64_t fenceValue;

		Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> resolveBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView;

		D3D12_RESOURCE_BARRIER renderBarrier;
		std::vector<D3D12_RESOURCE_BARRIER> resolveBarriers;
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

	void FrameBuffer::setResources(Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer, Microsoft::WRL::ComPtr<ID3D12Resource2> resolveBuffer, Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView) {
		implementation->renderTargetBuffer = renderTargetBuffer;
		implementation->resolveBuffer = resolveBuffer;
		implementation->constantBuffer = constantBuffer;
		implementation->renderTargetView = renderTargetView;
		debug::print("Resources set");

		implementation->renderBarrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->renderTargetBuffer.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		implementation->resolveBarriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(implementation->renderTargetBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE));
		implementation->resolveBarriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(implementation->resolveBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST));
		implementation->presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->resolveBuffer.Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT);
		debug::print("Barriers set");
	}
	
	void FrameBuffer::wait() {
		if (implementation->fence->GetCompletedValue() < implementation->fenceValue) {
			const auto fenceEvent = queue::getFenceEvent();

			debug::verify::com(implementation->fence->SetEventOnCompletion(implementation->fenceValue, fenceEvent));
			debug::verify::com(WaitForSingleObject(fenceEvent, INFINITE));
		}
	}

	void FrameBuffer::begin(D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView) {
		const auto commandList = queue::getCommandList();
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		debug::verify::com(implementation->commandAllocator->Reset());
		debug::verify::com(commandList->Reset(implementation->commandAllocator.Get(), nullptr));
		
		commandList->ResourceBarrier(1, &implementation->renderBarrier);
		commandList->OMSetRenderTargets(1, &implementation->renderTargetView, false, &depthStencilView);
		commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);
		commandList->ClearRenderTargetView(implementation->renderTargetView, clearColor, 0, nullptr);
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> FrameBuffer::getConstantBuffer() {
		return implementation->constantBuffer;
	}
	
	void FrameBuffer::end() {
		const auto commandList = queue::getCommandList();

		commandList->ResourceBarrier(static_cast<uint32_t>(implementation->resolveBarriers.size()), implementation->resolveBarriers.data());
		commandList->ResolveSubresource(implementation->resolveBuffer.Get(), 0, implementation->renderTargetBuffer.Get(), 0, swapChain::getRenderTargetFormat());
		commandList->ResourceBarrier(1, &implementation->presentBarrier);

		debug::verify::com(commandList->Close());
	}

	void FrameBuffer::signal() {
		implementation->fenceValue++;
		debug::verify::com(queue::getCommandQueue()->Signal(implementation->fence.Get(), implementation->fenceValue));
	}
	
	FrameBuffer::~FrameBuffer() = default;
}
