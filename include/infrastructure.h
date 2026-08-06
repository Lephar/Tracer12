#pragma once

#include "pch.h"

namespace tracer::infrastructure {
	void initialize();

	Microsoft::WRL::ComPtr<IDXGIFactory7> getFactory();
	Microsoft::WRL::ComPtr<IDXGIAdapter4> getAdapter();
}
