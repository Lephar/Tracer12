#include "pch.h"

#include "queue.h"

#include "verify.h"

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
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 1,
		};

		VERIFY_COM(device->CreateCommandQueue1(&commandQueueDesc, {}, IID_PPV_ARGS(commandQueue.GetAddressOf())));
		std::println("Direct command queue created");

		VERIFY_COM(commandQueue->QueryInterface(IID_PPV_ARGS(debugCommandQueue.GetAddressOf())));
		std::println("Debug command queue created from command queue");

		VERIFY_COM(device->CreateCommandList1(1, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(commandList.GetAddressOf())));
		std::println("Direct command list created");

		VERIFY_COM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
		std::println("Command allocator created");

		fenceValue = 0;
		VERIFY_COM(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
		std::println("Fence created and value set");

		fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		VERIFY_WIN(fenceEvent);
		std::println("Fence event created");
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue1> getCommandQueue() {
		return commandQueue;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList() {
		return commandList;
	}

	void begin() {
		VERIFY_COM(commandAllocator->Reset());
		VERIFY_COM(commandList->Reset(commandAllocator.Get(), nullptr));
		std::println("Command allocator and list reset");
	}

	void end() {
		VERIFY_COM(commandList->Close());
		std::println("Command list closed");
	}

	void execute() {
		commandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(commandList.GetAddressOf()));
		std::println("Commands sent for execution");
	}

	void signal() {
		fenceValue++;
		VERIFY_COM(commandQueue->Signal(fence.Get(), fenceValue));
		std::println("Fence signal sent");
	}

	void wait() {
		if (fence->GetCompletedValue() < fenceValue) {
			VERIFY_COM(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			VERIFY_COM(WaitForSingleObject(fenceEvent, INFINITE));
		}

		std::println("Commands completed");
	}
}
