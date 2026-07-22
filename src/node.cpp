#include "pch.h"

#include "node.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Node::Implementation {
		std::optional<uint32_t> meshIndex;
		std::optional<uint32_t> cameraIndex;
		std::optional<uint32_t> lightIndex;

		std::vector<Node> children;
	};

	Node::Node(cgltf_node* data) : implementation(std::make_unique<Implementation>()) {
		debug::print("Node: %s", data->name);
		debug::incrementDepth();

		cgltf_float transform[16];
		if (data->mesh || data->camera || data->light) {
			cgltf_node_transform_world(data, transform);
		}

		if (data->mesh) {
			auto& meshes = getMeshes();
			implementation->meshIndex = static_cast<uint32_t>(meshes.size());
			meshes.emplace_back(data->mesh, transform);
		}

		if (data->camera && data->camera->type == cgltf_camera_type_perspective) {
			auto& cameras = getCameras();
			implementation->cameraIndex = static_cast<uint32_t>(cameras.size());
			cameras.emplace_back(data->camera, transform);
		}

		if (data->light && data->light->type == cgltf_light_type_point) {
			auto& lights = getLights();
			implementation->lightIndex = static_cast<uint32_t>(lights.size());
			lights.emplace_back(data->light, transform);
		}

		implementation->children.reserve(data->children_count);

		for (cgltf_size childIndex = 0; childIndex < data->children_count; childIndex++) {
			cgltf_node* childData = data->children[childIndex];
			implementation->children.emplace_back(childData);
		}

		debug::decrementDepth();
	}

	Node::Node(Node&& node) noexcept : implementation(std::move(node.implementation)) {}

	Node& Node::operator=(Node&& node) noexcept {
		implementation = std::move(node.implementation);
		return *this;
	}
	
	void Node::draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		if (implementation->meshIndex.has_value()) {
			getMeshes().at(implementation->meshIndex.value()).draw(commandList);
		}

		for (auto& child : implementation->children) {
			child.draw(commandList);
		}
	}
	
	Node::~Node() = default;
}
