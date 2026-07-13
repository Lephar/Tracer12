#pragma once

#include "pch.h"

namespace tracer::debug {
#ifdef _DEBUG
	constexpr bool enabled = true;
#else
	constexpr bool enabled = false;
#endif

	void activate();
	void deactivate();

	void incrementDepth();
	void decrementDepth();

	void print(const char* fmt, ...);

	namespace verify {
		void positive(bool result);
		void negative(bool result);
		void win(bool result);
		void com(HRESULT result);
		void dxc(Microsoft::WRL::ComPtr<IDxcResult> result);
		void gltf(cgltf_result result);
	}
}
