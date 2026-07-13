#pragma once

#include "pch.h"

namespace tracer::content {
	class Light {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		struct Constant {
			DirectX::SimpleMath::Vector4 position;
			DirectX::SimpleMath::Vector4 color;
		};

		Light(cgltf_light* data, cgltf_float* transform);

		Light(const Light& light) = delete;
		Light& operator=(const Light& light) = delete;

		Light(Light&& light) noexcept;
		Light& operator=(Light&& light) noexcept;

		~Light();
	};
}
