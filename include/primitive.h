#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	class Material;

	class Primitive {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Primitive(cgltf_primitive* data, std::vector<Material>& materials);

		Primitive(const Primitive& primitive) = delete;
		Primitive& operator=(const Primitive& primitive) = delete;

		Primitive(Primitive&& primitive) noexcept;
		Primitive& operator=(Primitive&& primitive) noexcept;

		~Primitive();
	};
}
