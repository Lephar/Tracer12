#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	class Material;

	class Mesh {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Mesh(cgltf_mesh* data, cgltf_float* transform, std::vector<Material>& materials);

		Mesh(const Mesh& mesh) = delete;
		Mesh& operator=(const Mesh& mesh) = delete;

		Mesh(Mesh&& mesh) noexcept;
		Mesh& operator=(Mesh&& mesh) noexcept;

		void draw();

		~Mesh();
	};
}
