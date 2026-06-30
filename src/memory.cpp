#include "pch.h"

#include "memory.h"

#include "graphics.h"
#include "swapChain.h"

#include "verify.h"

namespace tracer::graphics::memory {
	namespace {
		std::shared_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap = nullptr;
		std::shared_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap = nullptr;
		std::shared_ptr<DirectX::DescriptorHeap> constantBufferDescriptorHeap = nullptr;

		CD3DX12_HEAP_PROPERTIES defaultHeapProperties = {};
		CD3DX12_HEAP_PROPERTIES uploadHeapProperties = {};
	}

	void initialize() {
		auto device = getDevice();
		const auto imageCount = swapChain::getImageCount();
		
		depthStencilDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
		std::println("Depth stencil descriptor heap created");

		renderTargetDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, imageCount);
		std::println("Render target descriptor heap created");

		constantBufferDescriptorHeap = std::make_shared<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, imageCount);
		std::println("Constant buffer descriptor heap created");

		defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		std::println("Default heap properties set");

		uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		std::println("Upload heap properties set");
	}

	std::shared_ptr<DirectX::DescriptorHeap> getDepthStencilDescriptorHeap() {
		return depthStencilDescriptorHeap;
	}

	std::shared_ptr<DirectX::DescriptorHeap> getRenderTargetDescriptorHeap() {
		return renderTargetDescriptorHeap;
	}

	std::shared_ptr<DirectX::DescriptorHeap> getConstantBufferDescriptorHeap() {
		return constantBufferDescriptorHeap;
	}

	CD3DX12_HEAP_PROPERTIES getDefaultHeapProperties() {
		return defaultHeapProperties;
	}

	CD3DX12_HEAP_PROPERTIES getUploadHeapProperties() {
		return uploadHeapProperties;
	}
}
