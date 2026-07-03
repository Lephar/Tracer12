#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	class Material;

	class Node {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Node(cgltf_node* data, std::vector<Material>& materials);

		Node(const Node& node) = delete;
		Node& operator=(const Node& node) = delete;

		Node(Node&& node) noexcept;
		Node& operator=(Node&& node) noexcept;

		~Node();
	};
}
