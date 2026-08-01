#pragma once

#include "pch.h"

namespace tracer::graphics {
	class Pipeline {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;

	public:
		Pipeline(
			Microsoft::WRL::ComPtr<ID3D12Device15> device,
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
			Microsoft::WRL::ComPtr<IDxcBlob> vertexShader,
			Microsoft::WRL::ComPtr<IDxcBlob> pixelShader,
			uint32_t sampleCount,
			DXGI_FORMAT depthStencilFormat,
			DXGI_FORMAT renderTargetFormat,
			bool blending,
			bool culling
		);

		Pipeline(const Pipeline& pipeline) = delete;
		Pipeline& operator=(const Pipeline& pipeline) = delete;

		Pipeline(Pipeline&& pipeline) noexcept;
		Pipeline& operator=(Pipeline&& pipeline) noexcept;

		void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

		~Pipeline();
	};
}
