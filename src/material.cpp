#include "pch.h"

#include "material.h"
#include "texture.h"

namespace tracer::graphics::content {
	struct Material::Implementation {
		std::optional<Texture> baseColor;
		std::optional<Texture> metallicRoughness;
		std::optional<Texture> normal;
	};

	Material::Material(const char* folder, cgltf_material* data) : implementation(std::make_unique<Implementation>()) {
		std::println("\tMaterial name: {}", data->name);

		if (data->has_pbr_metallic_roughness) {
			if (data->pbr_metallic_roughness.base_color_texture.texture && data->pbr_metallic_roughness.base_color_texture.texture->image) {
				std::println("\t\tBase color texture:");
				implementation->baseColor.emplace(folder, data->pbr_metallic_roughness.base_color_texture.texture->image);
			}
			if (data->pbr_metallic_roughness.metallic_roughness_texture.texture && data->pbr_metallic_roughness.metallic_roughness_texture.texture->image) {
				std::println("\t\tMetallic roughness texture:");
				implementation->metallicRoughness.emplace(folder, data->pbr_metallic_roughness.metallic_roughness_texture.texture->image);
			}
		}
		if (data->normal_texture.texture && data->normal_texture.texture->image) {
			std::println("\t\tNormal texture:");
			implementation->normal.emplace(folder, data->normal_texture.texture->image);
		}
	}

	Material::Material(Material&& material) noexcept : implementation(std::move(material.implementation)) {}

	Material& Material::operator=(Material&& material) noexcept {
		implementation = std::move(material.implementation);
		return *this;
	}

	Material::~Material() = default;
}
