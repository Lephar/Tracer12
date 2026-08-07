#pragma once

#include "pch.h"

namespace tracer::content {
	class Material {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		struct Constant {
			DirectX::SimpleMath::Vector4 baseColorFactor;
			DirectX::SimpleMath::Vector3 metallicRoughnessNormalFactor;
		};

		Material(std::filesystem::path folder, cgltf_material* data, cgltf_image* images);

		Material(const Material& material) = delete;
		Material& operator=(const Material& material) = delete;

		Material(Material&& material) noexcept;
		Material& operator=(Material&& material) noexcept;

		bool operator==(const std::string& name) const;

		bool blending();
		bool culling();

		void bind();

		~Material();
	};
}
