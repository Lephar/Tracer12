#pragma once

#include "pch.h"

namespace tracer::graphics::compiler {
	void initialize(std::filesystem::path dataFolder);

	Microsoft::WRL::ComPtr<IDxcCompiler3> getCompiler();
	Microsoft::WRL::ComPtr<IDxcUtils> getUtils();

	std::filesystem::path getShaderFolder();

	Microsoft::WRL::ComPtr<IDxcBlob> loadShader(LPCWSTR name, LPCWSTR type, LPCWSTR entry);
}
