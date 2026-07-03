#include "pch.h"

#include "mesh.h"
#include "primitive.h"

namespace tracer::graphics::content {
	struct Mesh::Implementation {
		std::vector<Primitive> primitives;
	};

	Mesh::Mesh(cgltf_mesh* data, std::vector<Material>& materials) : implementation(std::make_unique<Implementation>()) {
		std::println("\t\tMesh name: {}", data->name);

		for (uint32_t primitiveIndex = 0; primitiveIndex < data->primitives_count; primitiveIndex++) {
			cgltf_primitive* primitiveData = &data->primitives[primitiveIndex];
			std::println("\t\t\tPrimitive {}", primitiveIndex);
			Primitive primitive{ primitiveData, materials };
		}
	}

	Mesh::Mesh(Mesh&& mesh) noexcept : implementation(std::move(mesh.implementation)) {}

	Mesh& Mesh::operator=(Mesh&& mesh) noexcept {
		implementation = std::move(mesh.implementation);
		return *this;
	}

	Mesh::~Mesh() = default;
}
