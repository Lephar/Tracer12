#pragma once

#include "pch.h"

namespace tracer::pipeline {
	void create();

	Microsoft::WRL::ComPtr<ID3D12PipelineState> getPipelineState(bool blending, bool culling);
}
