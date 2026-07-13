#pragma once

#include "pch.h"

namespace tracer::graphics {
	void initialize(std::filesystem::path dataFolder, HWND window, uint32_t width, uint32_t height);
	
	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList();

	void createResources(uint32_t materialCount, uint32_t materialTextureCount);

	D3D12_HEAP_PROPERTIES getDefaultHeapProperties();
	D3D12_HEAP_PROPERTIES getUploadHeapProperties();

	std::shared_ptr<DirectX::DescriptorHeap> getDepthStencilDescriptorHeap();
	std::shared_ptr<DirectX::DescriptorHeap> getRenderTargetDescriptorHeap();
	std::shared_ptr<DirectX::DescriptorHeap> getShaderResourceDescriptorHeap();

	void beginCommand();
	void endCommand();
}
