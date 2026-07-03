#include "pch.h"

#include "texture.h"

#include "system.h"

namespace tracer::graphics::content {
	struct Texture::Implementation {
	
	};

	Texture::Texture(const char* folder, cgltf_image *data) : implementation(std::make_unique<Implementation>()) {
		if (data->name) {
			std::println("\t\t\tName: {}", data->name);
		}

		auto path = system::getDataFolder() / "assets" / folder / data->uri;
		std::println("\t\t\tPath: {}", path.string());
	}

	Texture::~Texture() = default;
}
