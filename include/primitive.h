#pragma once

#include "pch.h"

namespace tracer::content {
	class Primitive {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		typedef uint32_t Index;

		struct Vertex {
			DirectX::SimpleMath::Vector4 position;
			DirectX::SimpleMath::Vector4 tangent;
			DirectX::SimpleMath::Vector4 normal;
			DirectX::SimpleMath::Vector4 texcoord;
		};

		Primitive(cgltf_primitive* data);

		Primitive(const Primitive& primitive) = delete;
		Primitive& operator=(const Primitive& primitive) = delete;

		Primitive(Primitive&& primitive) noexcept;
		Primitive& operator=(Primitive&& primitive) noexcept;

		void draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

		~Primitive();
	};
}
