#include "pch.h"

#include "camera.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Camera::Implementation {
		float fieldOfView;
		float aspectRatio;
		float nearPlane;
		float farPlane;

		uint32_t constantIndex;
	};

	Camera::Camera(cgltf_camera* data, cgltf_float* transform) : implementation(std::make_unique<Implementation>()) {
		debug::print("Camera: %s", data->name);
		debug::incrementDepth();

		debug::verify::positive(data->type == cgltf_camera_type_perspective);
		auto& perspective = data->data.perspective;

		implementation->fieldOfView = perspective.yfov;
		debug::print("Field of view: %g", implementation->fieldOfView);

		implementation->aspectRatio = perspective.has_aspect_ratio ? perspective.aspect_ratio : 1.0f;
		debug::print("Aspect ratio:  %g", implementation->aspectRatio);

		implementation->nearPlane = perspective.znear;
		debug::print("Near plane:    %g", implementation->nearPlane);
		
		implementation->farPlane = perspective.has_zfar ? perspective.zfar : perspective.znear * 1024.0f;
		debug::print("Far plane:     %g", implementation->farPlane);

		auto view = DirectX::SimpleMath::Matrix{ transform }.Invert();
		auto projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(implementation->fieldOfView, implementation->aspectRatio, implementation->farPlane, implementation->nearPlane);
		auto projectionView = projection * view;

		auto& constants = getCameraConstants();
		implementation->constantIndex = static_cast<uint32_t>(constants.size());
		constants.emplace_back(view, projection, projectionView);

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
