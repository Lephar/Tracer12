#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	typedef uint32_t Index;

	struct Vertex {
		DirectX::SimpleMath::Vector4 position;
		DirectX::SimpleMath::Vector4 tangent;
		DirectX::SimpleMath::Vector4 normal;
		DirectX::SimpleMath::Vector4 texcoord;
	};

	struct Constant {
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix projection;
	};

	void load();

	std::vector<Index>& getIndices();
	std::vector<Vertex>& getVertices();
	Constant& getConstants();
	uint32_t getConstantBufferAlignment();
	Microsoft::WRL::ComPtr<ID3D12Resource2> getConstantBuffer();
	void* getConstantBufferMemory();

	void update();
	void draw();
}
