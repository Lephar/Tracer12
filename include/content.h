#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	typedef uint32_t Index;

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color;
	};

	struct Constant {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	void load();

	Constant& getConstants();
	uint32_t getConstantBufferAlignment();
	Microsoft::WRL::ComPtr<ID3D12Resource2> getConstantBuffer();
	void* getConstantBufferMemory();

	void draw();
}
