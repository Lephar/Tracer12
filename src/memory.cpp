#include "pch.h"

#include "memory.h"

#include "debug.h"

namespace tracer::graphics::memory {
	namespace {
		D3D12_HEAP_PROPERTIES defaultHeapProperties = {};
		D3D12_HEAP_PROPERTIES uploadHeapProperties = {};

		std::shared_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap = nullptr;
		std::shared_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap = nullptr;
		std::shared_ptr<DirectX::DescriptorHeap> shaderResourceDescriptorHeap = nullptr;
	}

	void allocate(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t imageCount, uint32_t materialCount, uint32_t materialTextureCount) {
		debug::print("Allocating memory:");
		debug::incrementDepth();

		auto shaderResourceCount = materialCount * materialTextureCount;

		defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		debug::print("Default heap properties set");

		uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		debug::print("Upload heap properties set");

		depthStencilDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
		debug::print("Depth stencil descriptor heap created with size 1");

		renderTargetDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, imageCount);
		debug::print("Render target descriptor heap created with size %u", imageCount);

		shaderResourceDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, shaderResourceCount);
		debug::print("Shader resource descriptor heap created with size %u", shaderResourceCount);

		debug::decrementDepth();
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
