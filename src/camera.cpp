#include "pch.h"
#include "camera.h"
#include "content.h"
#include "system.h"
#include "queue.h"
#include "rootSignature.h"
#include "debug.h"

namespace tracer::content {
	struct Camera::Implementation {
		float fieldOfView;
		float aspectRatio;
		float nearPlane;
		float farPlane;

		DirectX::SimpleMath::Matrix transform;

		uint32_t constantIndex;
	};

	Camera::Camera(cgltf_camera* data, cgltf_float* transform) : implementation(std::make_unique<Implementation>()) {
		debug::print("Camera: %s", data->name);
		debug::incrementDepth();

		debug::verify::positive(data->type == cgltf_camera_type_perspective);
		auto& perspective = data->data.perspective;

		implementation->fieldOfView = perspective.yfov;
		debug::print("Field of view: %g", implementation->fieldOfView);

		implementation->aspectRatio = static_cast<float>(system::getWidth()) / static_cast<float>(system::getHeight());
		debug::print("Aspect ratio:  %g", implementation->aspectRatio);

		if (perspective.has_aspect_ratio) {
			debug::print("Changed from:  %g", perspective.aspect_ratio);
		}

		implementation->nearPlane = perspective.znear;
		debug::print("Near plane:    %g", implementation->nearPlane);
		
		implementation->farPlane = perspective.has_zfar ? perspective.zfar : perspective.znear * 1024.0f;
		debug::print("Far plane:     %g", implementation->farPlane);

		auto matrix = DirectX::SimpleMath::Matrix{ transform };
		auto translation = DirectX::SimpleMath::Vector4::Transform(DirectX::SimpleMath::Vector4::UnitW, matrix);
		implementation->transform = DirectX::SimpleMath::Matrix::CreateTranslation(translation.x, translation.y, translation.z);
		
		auto& constants = getCameraConstants();
		implementation->constantIndex = static_cast<uint32_t>(constants.size());
		constants.push_back(Camera::Constant{});

		auto& constant = constants.back();
		constant.view = implementation->transform.Invert();
		constant.projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(implementation->fieldOfView, implementation->aspectRatio, implementation->farPlane, implementation->nearPlane);

		constant.properties = {
			implementation->fieldOfView,
			implementation->aspectRatio,
			implementation->nearPlane,
			implementation->farPlane,
		};
		
		debug::decrementDepth();
	}

	Camera::Camera(Camera&& camera) noexcept : implementation(std::move(camera.implementation)) {}

	Camera& Camera::operator=(Camera&& camera) noexcept {
		implementation = std::move(camera.implementation);
		return *this;
	}

	void Camera::update(DirectX::SimpleMath::Matrix transform, DirectX::SimpleMath::Matrix view) {
		auto& constant = getCameraConstants().at(implementation->constantIndex);

		constant.projectionView = constant.view * view * constant.projection;
		constant.position = DirectX::SimpleMath::Vector4::Transform(DirectX::SimpleMath::Vector4::UnitW, transform * implementation->transform);
	}

	void Camera::bind() {
		const auto commandList = queue::getCommandList();
		auto constantBufferView = getCurrentConstantBufferView() + getCameraConstantsOffset() + getCameraConstantAlignment() * implementation->constantIndex;

		commandList->SetGraphicsRootConstantBufferView(rootSignature::RootParameter::CameraConstantBufferView, constantBufferView);
	}

	Camera::~Camera() = default;
}
