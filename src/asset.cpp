#include "pch.h"

#include "asset.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Asset::Implementation {
		uint32_t materialOffset;
		std::vector<Node> nodes;
	};

	Asset::Asset(const char* folder, const char* file) : implementation(std::make_unique<Implementation>()) {
		debug::print("Asset: %s\\%s", folder, file);
		debug::incrementDepth();

		auto folderPath = getAssetFolder() / folder;
		auto filePath = folderPath / file;
		
		cgltf_data* data = nullptr;
		cgltf_options options = {};

		debug::verify::gltf(cgltf_parse_file(&options, filePath.string().c_str(), &data));
		debug::print("File parsed");

		debug::verify::gltf(cgltf_validate(data));
		debug::print("Asset validated");

		debug::verify::gltf(cgltf_load_buffers(&options, data, filePath.string().c_str()));
		debug::print("Buffers loaded");

		auto& materials = getMaterials();
		implementation->materialOffset = static_cast<uint32_t>(materials.size());

		for (cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
			cgltf_material* materialData = &data->materials[materialIndex];
			materials.emplace_back(folderPath, materialData);
		}

		auto& scene = data->scene;
		implementation->nodes.reserve(scene->nodes_count);

		for (cgltf_size nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
			cgltf_node* nodeData = scene->nodes[nodeIndex];
			implementation->nodes.emplace_back(nodeData);
		}

		cgltf_free(data);

		debug::decrementDepth();
	}

	Asset::Asset(Asset&& asset) noexcept : implementation(std::move(asset.implementation)) {}

	Asset& Asset::operator=(Asset&& asset) noexcept {
		implementation = std::move(asset.implementation);
		return *this;
	}
	/*
	void Asset::draw() {
		for (auto& node : implementation->nodes) {
			node.draw();
		}
	}
	*/
	Asset::~Asset() = default;
}
