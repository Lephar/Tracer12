#pragma once

#include "pch.h"

namespace tracer::graphics {
	void initialize(std::filesystem::path dataFolder, HWND window, uint32_t width, uint32_t height);
	
	Microsoft::WRL::ComPtr<ID3D12Device15> getDevice();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> getCommandList();
	uint32_t getImageCount();

	void createResources(uint32_t constantBufferSize, uint32_t textureCount);

	void beginCommand();
	void endCommand();

	void beginFrame();
	Microsoft::WRL::ComPtr<ID3D12Resource2> getCurrentConstantBuffer();
	void endFrame();

	void destroy();
}
