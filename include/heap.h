#pragma once

#include "pch.h"

namespace tracer::graphics {
	class Heap {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Heap(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, uint64_t size, uint64_t alignment);
		
		Microsoft::WRL::ComPtr<ID3D12Resource2> place(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_RESOURCE_DESC1 resourceDesc, D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE clearValue);

		Heap(const Heap& heap) = delete;
		Heap& operator=(const Heap& heap) = delete;

		Heap(Heap&& heap) = delete;
		Heap& operator=(Heap&& heap) = delete;

		~Heap();
	};
}
