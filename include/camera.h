#pragma once

#include "pch.h"

namespace tracer::content {
	class Camera {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		struct Constant {
			DirectX::SimpleMath::Matrix view;
			DirectX::SimpleMath::Matrix projection;
			DirectX::SimpleMath::Matrix projectionView;
		};

		Camera(cgltf_camera* data, cgltf_float* transform);

		Camera(const Camera& camera) = delete;
		Camera& operator=(const Camera& camera) = delete;

		Camera(Camera&& camera) noexcept;
		Camera& operator=(Camera&& camera) noexcept;

		~Camera();
	};
}
