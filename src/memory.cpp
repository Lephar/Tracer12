#include "pch.h"

#include "memory.h"

#include "verify.h"

namespace tracer::graphics::memory {
	namespace {
		D3D12_HEAP_PROPERTIES defaultHeapProperties = {};
		D3D12_HEAP_PROPERTIES uploadHeapProperties = {};

		std::shared_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap = nullptr;
		std::shared_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap = nullptr;
		std::shared_ptr<DirectX::DescriptorHeap> shaderResourceDescriptorHeap = nullptr;
	}

	void allocate(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t imageCount, uint32_t materialCount, uint32_t materialTextureCount) {
		auto shaderResourceCount = materialCount * materialTextureCount;

		defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		std::println("Default heap properties set");

		uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		std::println("Upload heap properties set");

		depthStencilDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
		std::println("Depth stencil descriptor heap created with size 1");

		renderTargetDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, imageCount);
		std::println("Render target descriptor heap created with size {}", imageCount);

		shaderResourceDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, shaderResourceCount);
		std::println("Shader resource descriptor heap created with size {}", shaderResourceCount);
	}

	D3D12_HEAP_PROPERTIES getDefaultHeapProperties() {
		return defaultHeapProperties;
	}

	D3D12_HEAP_PROPERTIES getUploadHeapProperties() {
		return uploadHeapProperties;
	}

	std::shared_ptr<DirectX::DescriptorHeap> getDepthStencilDescriptorHeap() {
		return depthStencilDescriptorHeap;
	}

	std::shared_ptr<DirectX::DescriptorHeap> getRenderTargetDescriptorHeap() {
		return renderTargetDescriptorHeap;
	}

	std::shared_ptr<DirectX::DescriptorHeap> getShaderResourceDescriptorHeap() {
		return shaderResourceDescriptorHeap;
	}
}
