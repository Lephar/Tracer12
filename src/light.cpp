#include "pch.h"

#include "light.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Light::Implementation {
		uint32_t constantIndex;
	};

	void Light::bindAll(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		const auto lightConstantBufferView = getCurrentConstantBufferView() + getLightConstantsOffset();
		const auto lightConstantCount = static_cast<uint32_t>(getLightConstants().size());

		commandList->SetGraphicsRootConstantBufferView(2, lightConstantBufferView);
		commandList->SetGraphicsRoot32BitConstant(3, lightConstantCount, 0);
	}

	Light::Light(cgltf_light* data, cgltf_float* transform) : implementation(std::make_unique<Implementation>()) {
		debug::print("Light: %s", data->name);
		debug::incrementDepth();

		debug::verify::positive(data->type == cgltf_light_type_point);

		auto position = DirectX::SimpleMath::Vector4::Transform(DirectX::SimpleMath::Vector4::UnitW, DirectX::SimpleMath::Matrix{ transform });
		debug::print("Position: [%g, %g, %g, %g]", position.x, position.y, position.z, position.w);

		auto color = DirectX::SimpleMath::Vector4 { data->color[0], data->color[1], data->color[2], data->intensity > 0.0f ? data->intensity : 1.0f };
		debug::print("Color:    [%g, %g, %g, %g]", color.x, color.y, color.z, color.w);

		auto& constants = getLightConstants();
		implementation->constantIndex = static_cast<uint32_t>(constants.size());
		constants.emplace_back(position, color);

		debug::decrementDepth();
	}

	Light::Light(Light&& light) noexcept : implementation(std::move(light.implementation)) {}

	Light& Light::operator=(Light&& light) noexcept {
		implementation = std::move(light.implementation);
		return *this;
	}

	Light::~Light() = default;
}
