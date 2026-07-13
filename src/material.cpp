#include "pch.h"

#include "material.h"

#include "content.h"

namespace tracer::content {
	struct Material::Implementation {
		std::optional<cgltf_size> baseColorIndex;
		std::optional<cgltf_size> metallicRoughnessIndex;
		std::optional<cgltf_size> normalIndex;
	};

	uint32_t Material::getTextureCount() {
		return sizeof(Implementation) / sizeof(std::optional<cgltf_size>);
	}

	Material::Material(const char* folder, cgltf_material* data) : implementation(std::make_unique<Implementation>()) {
		std::println("\tMaterial name: {}", data->name);

		auto& textures = getTextures();

		if (data->has_pbr_metallic_roughness) {
			if (data->pbr_metallic_roughness.base_color_texture.texture && data->pbr_metallic_roughness.base_color_texture.texture->image) {
				std::println("\t\tBase color texture:");

				implementation->baseColorIndex.emplace(textures.size());
				textures.emplace_back(getAssetFolder() / folder, data->pbr_metallic_roughness.base_color_texture.texture->image, false);
			}
			if (data->pbr_metallic_roughness.metallic_roughness_texture.texture && data->pbr_metallic_roughness.metallic_roughness_texture.texture->image) {
				std::println("\t\tMetallic roughness texture:");
				
				implementation->metallicRoughnessIndex.emplace(textures.size());
				textures.emplace_back(getAssetFolder() / folder, data->pbr_metallic_roughness.metallic_roughness_texture.texture->image, false);
			}
		}
		if (data->normal_texture.texture && data->normal_texture.texture->image) {
			std::println("\t\tNormal texture:");
			
			implementation->normalIndex.emplace(textures.size());
			textures.emplace_back(getAssetFolder() / folder, data->normal_texture.texture->image, false);
		}
	}

	Material::Material(Material&& material) noexcept : implementation(std::move(material.implementation)) {}

	Material& Material::operator=(Material&& material) noexcept {
		implementation = std::move(material.implementation);
		return *this;
	}

	Material::~Material() = default;
}
