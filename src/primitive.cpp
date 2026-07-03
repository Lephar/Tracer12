#include "pch.h"

#include "primitive.h"

namespace tracer::graphics::content {
	struct Primitive::Implementation {
	};

	Primitive::Primitive(cgltf_primitive* data, std::vector<Material>& materials) : implementation(std::make_unique<Implementation>()) {
	}

	Primitive::Primitive(Primitive&& primitive) noexcept : implementation(std::move(primitive.implementation)) {}

	Primitive& Primitive::operator=(Primitive&& primitive) noexcept {
		implementation = std::move(primitive.implementation);
		return *this;
	}

	Primitive::~Primitive() = default;
}
