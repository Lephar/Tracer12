#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	typedef uint32_t Index;

	struct Vertex {
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 color;
	};

	struct Constant {
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix projection;
	};

	void load();

	Constant& getConstants();
	uint32_t getConstantBufferAlignment();
	Microsoft::WRL::ComPtr<ID3D12Resource2> getConstantBuffer();
	void* getConstantBufferMemory();

	void draw();
}
