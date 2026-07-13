#pragma once

#include "pch.h"

namespace tracer::graphics::swapChain {
	void initialize(
		HWND window,
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory,
		Microsoft::WRL::ComPtr<ID3D12Device15> device,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue1> queue,
		uint32_t swapChainWidth,
		uint32_t swapChainHeight,
		uint32_t swapChainImageCount,
		DXGI_FORMAT swapChainDepthStencilFormat,
		DXGI_FORMAT swapChainRenderTargetFormat
	);

	uint32_t getImageCount();

	DXGI_FORMAT getDepthStencilFormat();
	DXGI_FORMAT getRenderTargetFormat();

	void createResources(
		Microsoft::WRL::ComPtr<ID3D12Device15> device,
		D3D12_HEAP_PROPERTIES defaultHeapProperties,
		std::shared_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap,
		std::shared_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap
	);
	/*
	void begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);
	void end(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

	void bind();
	void present();

	void destroy();
	*/
}
