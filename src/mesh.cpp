#include "pch.h"

#include "mesh.h"
#include "primitive.h"

namespace tracer::graphics::content {
	struct Mesh::Implementation {
		std::vector<Primitive> primitives;
	};

	Mesh::Mesh(cgltf_mesh* data, cgltf_float* transform, std::vector<Material>& materials) : implementation(std::make_unique<Implementation>()) {
		std::println("\t\tMesh name: {}", data->name);

		for (uint32_t primitiveIndex = 0; primitiveIndex < data->primitives_count; primitiveIndex++) {
			cgltf_primitive* primitiveData = &data->primitives[primitiveIndex];
			std::println("\t\t\tPrimitive {}", primitiveIndex);
			implementation->primitives.emplace_back(primitiveData, materials);
		}
	}

	Mesh::Mesh(Mesh&& mesh) noexcept : implementation(std::move(mesh.implementation)) {}

	Mesh& Mesh::operator=(Mesh&& mesh) noexcept {
		implementation = std::move(mesh.implementation);
		return *this;
	}

	void Mesh::draw() {
		for (auto& primitive : implementation->primitives) {
			primitive.draw();
		}
	}

	Mesh::~Mesh() = default;
}
