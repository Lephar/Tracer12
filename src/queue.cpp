#include "pch.h"

#include "queue.h"

#include "debug.h"

namespace tracer::graphics::queue {
	namespace {
		Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DebugCommandQueue1> debugCommandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence = nullptr;
		HANDLE fenceEvent = nullptr;
		uint64_t fenceValue = 0;
	}

	void initialize(Microsoft::WRL::ComPtr<ID3D12Device15> device) {
		debug::print("Initializing queue:");
		debug::incrementDepth();

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 1,
		};

		debug::verify::com(device->CreateCommandQueue1(&commandQueueDesc, {}, IID_PPV_ARGS(commandQueue.GetAddressOf())));
		debug::print("Direct command queue created");

		if (debug::enabled) {
			debug::verify::com(commandQueue->QueryInterface(IID_PPV_ARGS(debugCommandQueue.GetAddressOf())));
			debug::print("Debug command queue created from command queue");
		}

		debug::verify::com(device->CreateCommandList1(1, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(commandList.GetAddressOf())));
		debug::print("Direct command list created");

		debug::verify::com(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
		debug::print("Command allocator created");

		fenceValue = 0;
		debug::verify::com(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
		debug::print("Fence created and value set");

		fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		debug::verify::win(fenceEvent);
		debug::print("Fence event created");

		debug::decrementDepth();
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue1> getCommandQueue() {
		return commandQueue;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList() {
		return commandList;
	}

	void begin() {
		debug::verify::com(commandAllocator->Reset());
		debug::verify::com(commandList->Reset(commandAllocator.Get(), nullptr));
	}

	void end() {
		debug::verify::com(commandList->Close());
	}

	void execute() {
		commandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(commandList.GetAddressOf()));
	}

	void signal() {
		fenceValue++;
		debug::verify::com(commandQueue->Signal(fence.Get(), fenceValue));
	}

	void wait() {
		if (fence->GetCompletedValue() < fenceValue) {
			debug::verify::com(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			debug::verify::com(WaitForSingleObject(fenceEvent, INFINITE));
		}
	}
}
