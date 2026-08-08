#include "pch.h"
#include "memory.h"
#include "heap.h"
#include "debug.h"

namespace tracer::memory {
	namespace {
		std::unique_ptr<Heap> uploadHeap;
		std::unique_ptr<Heap> bufferHeap;
		std::unique_ptr<Heap> renderTargetDepthStencilTextureHeap;
		std::unique_ptr<Heap> nonRenderTargetDepthStencilTexturesHeap;
	}

	void allocate(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint64_t buffersSize, uint64_t renderTargetDepthStencilTexturesSize, uint64_t nonRenderTargetDepthStencilTexturesSize) {
		uploadHeap = std::make_unique<Heap>(device, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, buffersSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
		debug::print("Upload heap created with size");

		bufferHeap = std::make_unique<Heap>(device, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, buffersSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
		debug::print("Buffer heap created with size");
		
		renderTargetDepthStencilTextureHeap = std::make_unique<Heap>(device, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES, renderTargetDepthStencilTexturesSize, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);
		debug::print("Render target / depth stencil texture heap created with size");
		
		nonRenderTargetDepthStencilTexturesHeap = std::make_unique<Heap>(device, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES, nonRenderTargetDepthStencilTexturesSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
		debug::print("Non render target / depth stencil textures heap created with size");
	}
}
