#pragma once

#include "pch.h"

namespace tracer::content {
	class Material {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Material();
		Material(std::filesystem::path folder, cgltf_material* data);

		Material(const Material& material) = delete;
		Material& operator=(const Material& material) = delete;

		Material(Material&& material) noexcept;
		Material& operator=(Material&& material) noexcept;

		bool operator==(const std::string& name) const;

		void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

		~Material();
	};
}
