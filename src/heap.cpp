#include "pch.h"

#include "heap.h"

#include "debug.h"

namespace tracer::graphics {
	struct Heap::Implementation {
		Microsoft::WRL::ComPtr<ID3D12Heap1> heap;
		uint64_t offset;
	};

	Heap::Heap(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, uint64_t size, uint64_t alignment) : implementation(std::make_unique<Implementation>()) {
		implementation->offset = 0;

		CD3DX12_HEAP_DESC heapDesc(size, type, alignment, flags);
		debug::verify::com(device->CreateHeap1(&heapDesc, nullptr, IID_PPV_ARGS(implementation->heap.GetAddressOf())));
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> Heap::place(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_RESOURCE_DESC1 resourceDesc, D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE clearValue) {
		D3D12_RESOURCE_ALLOCATION_INFO1 allocationInfo;
		device->GetResourceAllocationInfo2(1, 1, &resourceDesc, &allocationInfo);
		
		Microsoft::WRL::ComPtr<ID3D12Resource2> resource;
		device->CreatePlacedResource1(implementation->heap.Get(), implementation->offset, &resourceDesc, initialState, &clearValue, IID_PPV_ARGS(resource.GetAddressOf()));

		implementation->offset = allocationInfo.Offset;
		return resource;
	}

	Heap::~Heap() = default;
}
