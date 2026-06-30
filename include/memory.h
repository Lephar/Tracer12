#pragma once

#include "pch.h"

namespace tracer::graphics::memory {
	void initialize();

	std::shared_ptr<DirectX::DescriptorHeap> getDepthStencilDescriptorHeap();
	std::shared_ptr<DirectX::DescriptorHeap> getRenderTargetDescriptorHeap();
	std::shared_ptr<DirectX::DescriptorHeap> getConstantBufferDescriptorHeap();

	CD3DX12_HEAP_PROPERTIES getDefaultHeapProperties();
	CD3DX12_HEAP_PROPERTIES getUploadHeapProperties();
}
