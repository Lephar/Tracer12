#pragma once

#include "pch.h"

namespace tracer::graphics::content {
	class Texture {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Texture(const char* folder, cgltf_image* data);
		~Texture();
	};
}
