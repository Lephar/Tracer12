#pragma once

#include "pch.h"

namespace tracer::graphics {
	void initialize();

	Microsoft::WRL::ComPtr<IDxcCompiler3> getCompiler();
	Microsoft::WRL::ComPtr<IDxcUtils> getCompilerUtils();
	Microsoft::WRL::ComPtr<IDXGIFactory7> getFactory();
	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice();
	Microsoft::WRL::ComPtr<ID3D12CommandQueue1> getCommandQueue();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList();
}
