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
			DirectX::SimpleMath::Vector4 position;
			DirectX::SimpleMath::Vector4 properties;
		};

		Camera(cgltf_camera* data, cgltf_float* transform);

		Camera(const Camera& camera) = delete;
		Camera& operator=(const Camera& camera) = delete;

		Camera(Camera&& camera) noexcept;
		Camera& operator=(Camera&& camera) noexcept;

		void adjust(float aspectRatio);
		void update(DirectX::SimpleMath::Matrix transform, DirectX::SimpleMath::Matrix view);
		void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

		~Camera();
	};
}
