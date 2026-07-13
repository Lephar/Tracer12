#include "pch.h"

#include "camera.h"

#include "debug.h"

namespace tracer::content {
	struct Camera::Implementation {
	};

	Camera::Camera(cgltf_camera* data) : implementation(std::make_unique<Implementation>()) {
		debug::print("Camera: %s", data->name);
		debug::incrementDepth();

		debug::decrementDepth();
	}
	/*
	void aaaaaaaaaa() {
		const auto fieldOfView = DirectX::XM_PIDIV4;
		const auto aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		const auto nearPlane = 0.25f;
		const auto farPlane = 256.0f;

		projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, farPlane, nearPlane);

		debug::print("Projection matrix generated");
	}
	*/
	Camera::Camera(Camera&& camera) noexcept : implementation(std::move(camera.implementation)) {}

	Camera& Camera::operator=(Camera&& camera) noexcept {
		implementation = std::move(camera.implementation);
		return *this;
	}

	Camera::~Camera() = default;
}
