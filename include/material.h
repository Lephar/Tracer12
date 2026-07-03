#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	class Material {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Material(const char* folder, cgltf_material* data);

		Material(const Material& material) = delete;
		Material& operator=(const Material& material) = delete;

		Material(Material&& material) noexcept;
		Material& operator=(Material&& material) noexcept;

		~Material();
	};
}
