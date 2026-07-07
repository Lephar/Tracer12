#include "pch.h"

#include "asset.h"
#include "node.h"
#include "material.h"

#include "system.h"

#include "verify.h"

namespace tracer::graphics::content {
	struct Asset::Implementation {
		std::vector<Material> materials;
		std::vector<Node> nodes;
	};

	Asset::Asset(const char* folder, const char* name) : implementation(std::make_unique<Implementation>()) {
		std::println("Asset name: {}", name);

		auto path = system::getDataFolder() / "assets" / folder / (std::string(name) + ".gltf");
		std::println("\tPath: {}", path.string());

		cgltf_data* data = nullptr;
		cgltf_options options = {};

		VERIFY_GLTF(cgltf_parse_file(&options, path.string().c_str(), &data));
		std::println("\tFile parsed");

		VERIFY_GLTF(cgltf_validate(data));
		std::println("\tAsset validated");

		VERIFY_GLTF(cgltf_load_buffers(&options, data, path.string().c_str()));
		std::println("\tBuffers loaded");

		for (cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
			cgltf_material* materialData = &data->materials[materialIndex];
			//implementation->materials.emplace_back(folder, materialData);
		}

		cgltf_scene* scene = data->scene;

		for (cgltf_size nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
			cgltf_node* nodeData = scene->nodes[nodeIndex];
			implementation->nodes.emplace_back(nodeData, implementation->materials);
		}

		cgltf_free(data);
	}

	Asset::Asset(Asset&& asset) noexcept : implementation(std::move(asset.implementation)) {}

	Asset& Asset::operator=(Asset&& asset) noexcept {
		implementation = std::move(asset.implementation);
		return *this;
	}

	void Asset::draw() {
		for (auto& node : implementation->nodes) {
			node.draw();
		}
	}

	Asset::~Asset() = default;
}
