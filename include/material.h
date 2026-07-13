#pragma once

#include "pch.h"

namespace tracer::content {
	class Material {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		static uint32_t getTextureCount();

		Material(std::filesystem::path folder, cgltf_material* data);

		Material(const Material& material) = delete;
		Material& operator=(const Material& material) = delete;

		Material(Material&& material) noexcept;
		Material& operator=(Material&& material) noexcept;

		~Material();
	};
}
