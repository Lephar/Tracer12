#include "pch.h"

#include "node.h"
#include "mesh.h"

namespace tracer::graphics::content {
	struct Node::Implementation {
		std::optional<Mesh> mesh;
		std::vector<Node> children;
	};

	Node::Node(cgltf_node* data, std::vector<Material>& materials) : implementation(std::make_unique<Implementation>()) {
		std::println("\tNode name: {}", data->name);

		if (data->mesh) {
			cgltf_float transform[16];
			cgltf_node_transform_world(data, transform);
			implementation->mesh.emplace(data->mesh, transform, materials);
		}

		for (cgltf_size childIndex = 0; childIndex < data->children_count; childIndex++) {
			cgltf_node* childData = data->children[childIndex];
			implementation->children.emplace_back(childData, materials);
		}
	}

	Node::Node(Node&& node) noexcept : implementation(std::move(node.implementation)) {}

	Node& Node::operator=(Node&& node) noexcept {
		implementation = std::move(node.implementation);
		return *this;
	}

	void Node::draw() {
		if (implementation->mesh.has_value()) {
			implementation->mesh->draw();
		}

		for (auto& child : implementation->children) {
			child.draw();
		}
	}

	Node::~Node() = default;
}
