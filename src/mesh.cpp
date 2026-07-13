#include "pch.h"

#include "mesh.h"

#include "content.h"

namespace tracer::content {
	struct Mesh::Implementation {
		cgltf_size constantIndex;
		cgltf_size primitiveBegin;
		cgltf_size primitiveCount;
	};

	Mesh::Mesh(cgltf_mesh* data, cgltf_float* transform) : implementation(std::make_unique<Implementation>()) {
		std::println("\t\tMesh name: {}", data->name);

		auto& constants = getMeshConstants();
		auto& primitives = getPrimitives();

		implementation->constantIndex = constants.size();
		implementation->primitiveBegin = primitives.size();
		implementation->primitiveCount = data->primitives_count;

		for (uint32_t primitiveIndex = 0; primitiveIndex < implementation->primitiveCount; primitiveIndex++) {
			cgltf_primitive* primitiveData = &data->primitives[primitiveIndex];
			std::println("\t\t\tPrimitive {}", primitiveIndex);
			primitives.emplace_back(primitiveData);
		}
	}

	Mesh::Mesh(Mesh&& mesh) noexcept : implementation(std::move(mesh.implementation)) {}

	Mesh& Mesh::operator=(Mesh&& mesh) noexcept {
		implementation = std::move(mesh.implementation);
		return *this;
	}
	/*
	void Mesh::draw() {
		for (auto& primitive : implementation->primitives) {
			primitive.draw();
		}
	}
	*/
	Mesh::~Mesh() = default;
}
