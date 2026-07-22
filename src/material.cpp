#include "pch.h"

#include "material.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Material::Implementation {
		std::string name;

		uint32_t baseColorIndex;
		uint32_t metallicRoughnessIndex;
		uint32_t normalIndex;
	};

	Material::Material() : implementation(std::make_unique<Implementation>()) {
		implementation->name = std::string{ "Default" };
		
		debug::print("Material: %s", implementation->name.c_str());
		debug::incrementDepth();
		
		auto textureFolder = getAssetFolder() / implementation->name / "textures";
		auto& textures = getTextures();

		debug::print("Base color texture:");
		implementation->baseColorIndex = static_cast<uint32_t>(textures.size());
		textures.emplace_back(textureFolder, "baseColor.png", Texture::Type::BASE_COLOR);

		debug::print("Metallic roughness texture:");
		implementation->metallicRoughnessIndex = static_cast<uint32_t>(textures.size());
		textures.emplace_back(textureFolder, "metallicRoughness.png", Texture::Type::METALLIC_ROUGHNESS);

		debug::print("Normal texture:");
		implementation->normalIndex = static_cast<uint32_t>(textures.size());
		textures.emplace_back(textureFolder, "normal.png", Texture::Type::NORMAL);

		debug::decrementDepth();
	}

	Material::Material(std::filesystem::path folder, cgltf_material* data) : implementation(std::make_unique<Implementation>()) {
		debug::print("Material: %s", data->name);
		debug::incrementDepth();

		implementation->name = std::string{ data->name };

		auto& textures = getTextures();

		if (data->has_pbr_metallic_roughness && data->pbr_metallic_roughness.base_color_texture.texture && data->pbr_metallic_roughness.base_color_texture.texture->image) {
			debug::print("Base color texture:");

			implementation->baseColorIndex = static_cast<uint32_t>(textures.size());
			textures.emplace_back(folder, data->pbr_metallic_roughness.base_color_texture.texture->image, Texture::Type::BASE_COLOR);
		}
		else {
			debug::print("Using default base color texture");
			implementation->baseColorIndex = Texture::Type::BASE_COLOR;
		}
		if (data->has_pbr_metallic_roughness && data->pbr_metallic_roughness.metallic_roughness_texture.texture && data->pbr_metallic_roughness.metallic_roughness_texture.texture->image) {
			debug::print("Metallic roughness texture:");
				
			implementation->metallicRoughnessIndex = static_cast<uint32_t>(textures.size());
			textures.emplace_back(folder, data->pbr_metallic_roughness.metallic_roughness_texture.texture->image, Texture::Type::METALLIC_ROUGHNESS);
		}
		else {
			debug::print("Using default metallic roughness texture");
			implementation->metallicRoughnessIndex = Texture::Type::METALLIC_ROUGHNESS;
		}
		if (data->normal_texture.texture && data->normal_texture.texture->image) {
			debug::print("Normal texture:");
			
			implementation->normalIndex = static_cast<uint32_t>(textures.size());
			textures.emplace_back(folder, data->normal_texture.texture->image, Texture::Type::NORMAL);
		}
		else {
			debug::print("Using default normal texture");
			implementation->normalIndex = Texture::Type::NORMAL;
		}

		debug::decrementDepth();
	}

	Material::Material(Material&& material) noexcept : implementation(std::move(material.implementation)) {}

	Material& Material::operator=(Material&& material) noexcept {
		implementation = std::move(material.implementation);
		return *this;
	}

	bool Material::operator==(const std::string& name) const {
		return implementation->name == name;
	}

	void Material::bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		auto& textures = getTextures();

		const uint32_t constants[] = {
			implementation->baseColorIndex,
			implementation->metallicRoughnessIndex,
			implementation->normalIndex,
		};

		commandList->SetGraphicsRoot32BitConstants(3, Texture::Type::MAX_ENUM, constants, 1);
	}

	Material::~Material() = default;
}
