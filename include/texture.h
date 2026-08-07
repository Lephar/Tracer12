#pragma once

#include "pch.h"

namespace tracer::content {
	class Texture {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Texture(LPCWSTR path);

		Texture(const Texture& texture) = delete;
		Texture& operator=(const Texture& texture) = delete;

		Texture(Texture&& texture) noexcept;
		Texture& operator=(Texture&& texture) noexcept;

		void createResources(D3D12_CPU_DESCRIPTOR_HANDLE textureView);
		void recordUpload();
		void clearStaging();

		~Texture();
	};
}
