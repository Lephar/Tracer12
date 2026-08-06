#pragma once

#include "pch.h"

namespace tracer::swapChain {
	void initialize(
		HWND window,
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory,
		Microsoft::WRL::ComPtr<ID3D12Device15> device,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue1> queue,
		uint32_t swapChainWidth,
		uint32_t swapChainHeight
	);

	uint32_t getImageCount();
	uint32_t getSampleCount();

	DXGI_FORMAT getDepthStencilFormat();
	DXGI_FORMAT getRenderTargetFormat();

	void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t constantBufferSize);

	void begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, HANDLE fenceEvent);
	Microsoft::WRL::ComPtr<ID3D12Resource2> getCurrentConstantBuffer();
	void end(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);
	void present(Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue);

	void destroy(Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, HANDLE fenceEvent);
}
