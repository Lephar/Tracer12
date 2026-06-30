#pragma once

#include "pch.h"

namespace tracer::graphics {
	class Shader {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;

	public:
		Shader(const char* name, const char* type);

		Shader(const Shader& shader) = delete;
		Shader& operator=(const Shader& shader) = delete;

		Shader(Shader&& shader) = delete;
		Shader& operator=(Shader&& shader) = delete;

		D3D12_SHADER_BYTECODE getByteCode();

		~Shader();
	};
}
