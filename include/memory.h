#pragma once

#pragma once

#include "pch.h"

namespace tracer::graphics::memory {
	void allocate(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint64_t buffersSize, uint64_t renderTargetDepthStencilTexturesSize, uint64_t nonRenderTargetDepthStencilTexturesSize);
}
