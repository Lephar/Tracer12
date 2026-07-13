#pragma once

#include "pch.h"

namespace tracer::graphics {
	class Pipeline {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;

	public:
		Pipeline(Microsoft::WRL::ComPtr<ID3D12Device15> device, Microsoft::WRL::ComPtr<IDxcBlob> vertexShader, Microsoft::WRL::ComPtr<IDxcBlob> pixelShader, DXGI_FORMAT depthStencilFormat, DXGI_FORMAT renderTargetFormat);

		Pipeline(const Pipeline& pipeline) = delete;
		Pipeline& operator=(const Pipeline& pipeline) = delete;

		Pipeline(Pipeline&& pipeline) = delete;
		Pipeline& operator=(Pipeline&& pipeline) = delete;

		//void bind();

		~Pipeline();
	};
}
