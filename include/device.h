#pragma once

#include "pch.h"

namespace tracer::graphics::device {
	void initialize(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);

	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice();
}
