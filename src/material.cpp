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

	namespace {
		cgltf_image* getCompressedImage(cgltf_texture* texture, cgltf_image* images) {
			const char* extensionName = "MSFT_texture_dds";
			const uint32_t extensionNameLength = static_cast<uint32_t>(strnlen_s(extensionName, UCHAR_MAX));

			for (uint32_t extensionIndex = 0; extensionIndex < texture->extensions_count; extensionIndex++) {
				auto& extension = texture->extensions[extensionIndex];

				if (strncmp(extension.name, extensionName, extensionNameLength) == 0) {
					char discard[UCHAR_MAX];
					uint32_t source = UINT32_MAX;

					sscanf_s(extension.data, "%s %s %u", discard, UCHAR_MAX, discard, UCHAR_MAX, &source);
					debug::verify::positive(source != UINT32_MAX);

					return &images[source];
				}
			}

			return nullptr;
		}
	}

	Material::Material(std::filesystem::path folder, cgltf_material* data, cgltf_image* images) : implementation(std::make_unique<Implementation>()) {
		if (data) {
			implementation->name = std::string{ data->name };
		}
		else {
			implementation->name = std::string{ "default" };
		}

		debug::print("Material: %s", implementation->name.c_str());
		debug::incrementDepth();

		implementation->baseColorIndex = 0;
		implementation->metallicRoughnessIndex = 1;
		implementation->normalIndex = 2;

		auto& textures = getTextures();

		if (data) {
			if (data->has_pbr_metallic_roughness) {
				auto baseColorTexture = data->pbr_metallic_roughness.base_color_texture.texture;

				if (baseColorTexture) {
					auto baseColorImage = getCompressedImage(baseColorTexture, images);

					if (baseColorImage) {
						debug::print("Base color texture: %s", baseColorImage->name ? baseColorImage->name : baseColorImage->uri);
						auto baseColorPath = folder / baseColorImage->uri;
						implementation->baseColorIndex = static_cast<uint32_t>(textures.size());
						textures.emplace_back(baseColorPath.wstring().c_str());
					}
				}

				auto metallicRoughnessTexture = data->pbr_metallic_roughness.metallic_roughness_texture.texture;

				if (metallicRoughnessTexture) {
					auto metallicRoughnessImage = getCompressedImage(metallicRoughnessTexture, images);

					if (metallicRoughnessImage) {
						debug::print("Metallic roughness texture: %s", metallicRoughnessImage->name ? metallicRoughnessImage->name : metallicRoughnessImage->uri);
						auto metallicRoughnessPath = folder / metallicRoughnessImage->uri;
						implementation->metallicRoughnessIndex = static_cast<uint32_t>(textures.size());
						textures.emplace_back(metallicRoughnessPath.wstring().c_str());
					}
				}
			}

			auto normalTexture = data->normal_texture.texture;

			if (normalTexture) {
				auto normalImage = getCompressedImage(normalTexture, images);

				if (normalImage) {
					debug::print("Normal texture: %s", normalImage->name ? normalImage->name : normalImage->uri);
					auto normalPath = folder / normalImage->uri;
					implementation->normalIndex = static_cast<uint32_t>(textures.size());
					textures.emplace_back(normalPath.wstring().c_str());
				}
			}
		}
		else {
			debug::verify::positive(textures.empty());

			auto baseColorUri = "textures\\dds\\baseColor.dds";
			debug::print("Base color texture: %s", baseColorUri);
			auto baseColorPath = folder / baseColorUri;
			textures.emplace_back(baseColorPath.wstring().c_str());

			auto metallicRoughnessUri = "textures\\dds\\metallicRoughness.dds";
			debug::print("Metallic roughness texture: %s", metallicRoughnessUri);
			auto metallicRoughnessPath = folder / metallicRoughnessUri;
			textures.emplace_back(metallicRoughnessPath.wstring().c_str());

			auto normalUri = "textures\\dds\\normal.dds";
			debug::print("Normal texture: %s", normalUri);
			auto normalPath = folder / normalUri;
			textures.emplace_back(normalPath.wstring().c_str());
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

		const uint32_t constantCount = static_cast<uint32_t>(sizeof(constants) / sizeof(uint32_t));

		commandList->SetGraphicsRoot32BitConstants(3, constantCount, constants, 1);
	}

	Material::~Material() = default;
}
