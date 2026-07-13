#include "pch.h"

#include "material.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Material::Implementation {
		std::optional<uint32_t> baseColorIndex;
		std::optional<uint32_t> metallicRoughnessIndex;
		std::optional<uint32_t> normalIndex;
	};

	uint32_t Material::getTextureCount() {
		return sizeof(Implementation) / sizeof(std::optional<uint32_t>);
	}

	Material::Material(std::filesystem::path folder, cgltf_material* data) : implementation(std::make_unique<Implementation>()) {
		debug::print("Material: %s", data->name);
		debug::incrementDepth();

		auto& textures = getTextures();

		if (data->has_pbr_metallic_roughness) {
			if (data->pbr_metallic_roughness.base_color_texture.texture && data->pbr_metallic_roughness.base_color_texture.texture->image) {
				debug::print("Base color texture:");

				implementation->baseColorIndex.emplace(static_cast<uint32_t>(textures.size()));
				textures.emplace_back(folder, data->pbr_metallic_roughness.base_color_texture.texture->image, false);
			}
			if (data->pbr_metallic_roughness.metallic_roughness_texture.texture && data->pbr_metallic_roughness.metallic_roughness_texture.texture->image) {
				debug::print("Metallic roughness texture:");
				
				implementation->metallicRoughnessIndex.emplace(static_cast<uint32_t>(textures.size()));
				textures.emplace_back(folder, data->pbr_metallic_roughness.metallic_roughness_texture.texture->image, false);
			}
		}
		if (data->normal_texture.texture && data->normal_texture.texture->image) {
			debug::print("Normal texture:");
			
			implementation->normalIndex.emplace(static_cast<uint32_t>(textures.size()));
			textures.emplace_back(folder, data->normal_texture.texture->image, false);
		}

		debug::decrementDepth();
	}

	Material::Material(Material&& material) noexcept : implementation(std::move(material.implementation)) {}

	Material& Material::operator=(Material&& material) noexcept {
		implementation = std::move(material.implementation);
		return *this;
	}

	Material::~Material() = default;
}
