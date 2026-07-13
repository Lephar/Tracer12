#pragma once

#include "pch.h"

namespace tracer::content {
	class Texture {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Texture(std::filesystem::path folder, cgltf_image* data, bool compress);

		Texture(const Texture& texture) = delete;
		Texture& operator=(const Texture& texture) = delete;

		Texture(Texture&& texture) noexcept;
		Texture& operator=(Texture&& texture) noexcept;

		void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_HEAP_PROPERTIES uploadHeapProperties, D3D12_CPU_DESCRIPTOR_HANDLE textureView);
		void recordUpload(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);
		
		~Texture();
	};
}
