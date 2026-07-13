#pragma once

#include "pch.h"

namespace tracer::graphics::memory {
	void allocate(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t imageCount, uint32_t materialCount, uint32_t materialTextureCount);

	D3D12_HEAP_PROPERTIES getDefaultHeapProperties();
	D3D12_HEAP_PROPERTIES getUploadHeapProperties();

	std::shared_ptr<DirectX::DescriptorHeap> getDepthStencilDescriptorHeap();
	std::shared_ptr<DirectX::DescriptorHeap> getRenderTargetDescriptorHeap();
	std::shared_ptr<DirectX::DescriptorHeap> getShaderResourceDescriptorHeap();
}
