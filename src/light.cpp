#include "pch.h"

#include "light.h"

#include "debug.h"

namespace tracer::content {
	struct Light::Implementation {
		Constant constant;
	};

	Light::Light(cgltf_light* data) : implementation(std::make_unique<Implementation>()) {
		debug::print("Light: %s", data->name);
		debug::incrementDepth();

		debug::decrementDepth();
	}

	Light::Light(Light&& light) noexcept : implementation(std::move(light.implementation)) {}

	Light& Light::operator=(Light&& light) noexcept {
		implementation = std::move(light.implementation);
		return *this;
	}

	Light::~Light() = default;
}
