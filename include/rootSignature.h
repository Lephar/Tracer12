#pragma once

#include "pch.h"

namespace tracer::graphics::rootSignature {
	enum RootParameter {
		MeshConstantBufferView,
		CameraConstantBufferView,
		MaterialFactorConstantBufferView,
		LightConstantBufferView,
		ConstantIndexConstants,
		TexturesDescriptorTable,
	};
	
	void create(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t textureCount);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> getRootSignature();

	void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);
}
