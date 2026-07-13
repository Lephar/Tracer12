#include "pch.h"

#include "light.h"

namespace tracer::content {
	struct Light::Implementation {
		Constant constant;
	};

	Light::Light(cgltf_light* data) : implementation(std::make_unique<Implementation>()) {
		std::println("\t\tLight name: {}", data->name);
	}

	Light::Light(Light&& light) noexcept : implementation(std::move(light.implementation)) {}

	Light& Light::operator=(Light&& light) noexcept {
		implementation = std::move(light.implementation);
		return *this;
	}

	Light::~Light() = default;
}
