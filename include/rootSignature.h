#pragma once

#include "pch.h"

namespace tracer::rootSignature {
	enum RootParameter {
		MeshConstantBufferView,
		CameraConstantBufferView,
		MaterialFactorConstantBufferView,
		LightConstantBufferView,
		ConstantIndexConstants,
		TexturesDescriptorTable,
	};
	
	void create();

	Microsoft::WRL::ComPtr<ID3D12RootSignature> getRootSignature();
}
