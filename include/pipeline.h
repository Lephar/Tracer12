#pragma once

#include "pch.h"

namespace tracer {
	class Pipeline {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;

	public:
		Pipeline(Microsoft::WRL::ComPtr<IDxcBlob> vertexShader, Microsoft::WRL::ComPtr<IDxcBlob> pixelShader, bool blending, bool culling);

		Pipeline(const Pipeline& pipeline) = delete;
		Pipeline& operator=(const Pipeline& pipeline) = delete;

		Pipeline(Pipeline&& pipeline) noexcept;
		Pipeline& operator=(Pipeline&& pipeline) noexcept;

		void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

		~Pipeline();
	};
}
