#include "pch.h"

#include "mesh.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Mesh::Implementation {
		uint32_t constantIndex;
		uint32_t primitiveBegin;
		uint32_t primitiveCount;
	};

	Mesh::Mesh(cgltf_mesh* data, cgltf_float* transform) : implementation(std::make_unique<Implementation>()) {
		debug::print("Mesh: %s", data->name);
		debug::incrementDepth();

		auto& constants = getMeshConstants();
		
		implementation->constantIndex = static_cast<uint32_t>(constants.size());
		constants.emplace_back(DirectX::SimpleMath::Matrix{ transform });

		auto& primitives = getPrimitives();
		
		implementation->primitiveBegin = static_cast<uint32_t>(primitives.size());
		implementation->primitiveCount = static_cast<uint32_t>(data->primitives_count);

		for (uint32_t primitiveIndex = 0; primitiveIndex < implementation->primitiveCount; primitiveIndex++) {
			debug::print("Primitive: %u", primitiveIndex);
			debug::incrementDepth();

			cgltf_primitive* primitiveData = &data->primitives[primitiveIndex];
			primitives.emplace_back(primitiveData);
		
			debug::decrementDepth();
		}

		debug::decrementDepth();
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
