#pragma once

#include "pch.h"

namespace instance {
	void initialize();

	Microsoft::WRL::ComPtr<IDXGIFactory7> getFactory();
	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice();

	Microsoft::WRL::ComPtr<ID3D12CommandQueue1> getCommandQueue();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList();

	CD3DX12_HEAP_PROPERTIES getDefaultHeapProperties();
	CD3DX12_HEAP_PROPERTIES getUploadHeapProperties();
}
