#include "pch.h"

#include "node.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Node::Implementation {
		std::optional<cgltf_size> cameraIndex;
		std::optional<cgltf_size> lightIndex;
		std::optional<cgltf_size> meshIndex;

		std::vector<Node> children;
	};

	Node::Node(cgltf_node* data) : implementation(std::make_unique<Implementation>()) {
		debug::print("Node: %s", data->name);
		debug::incrementDepth();

		implementation->cameraIndex = {};
		implementation->lightIndex = {};

		if (data->mesh) {
			cgltf_float transform[16];
			cgltf_node_transform_world(data, transform);

			auto& meshes = getMeshes();
			implementation->meshIndex = meshes.size();
			meshes.emplace_back(data->mesh, transform);
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
	/*
	void Node::draw() {
		if (implementation->mesh.has_value()) {
			implementation->mesh->draw();
		}

		for (auto& child : implementation->children) {
			child.draw();
		}
	}
	*/
	Node::~Node() = default;
}
