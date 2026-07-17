#pragma once

#include "pch.h"

namespace tracer::content {
	class Mesh {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		struct Constant {
			DirectX::SimpleMath::Matrix model;
		};

		Mesh(cgltf_mesh* data, cgltf_float* transform);

		Mesh(const Mesh& mesh) = delete;
		Mesh& operator=(const Mesh& mesh) = delete;

		Mesh(Mesh&& mesh) noexcept;
		Mesh& operator=(Mesh&& mesh) noexcept;

		void draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

		~Mesh();
	};
}
