#include "pch.h"

#include "compiler.h"

#include "verify.h"

namespace tracer::graphics::compiler {
	namespace {
		std::filesystem::path shaderFolder = {};

		Microsoft::WRL::ComPtr<IDxcCompiler3> compiler = nullptr;
		Microsoft::WRL::ComPtr<IDxcUtils> utils = nullptr;

		std::vector<LPCWSTR> arguments = {};
	}

	void initialize(std::filesystem::path dataFolder) {
		shaderFolder = dataFolder / "shaders";
		std::println("Shader folder set: {}", shaderFolder.string());

		VERIFY_COM(DxcCreateInstance2(nullptr, CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
		std::println("Shader compiler created");

		VERIFY_COM(DxcCreateInstance2(nullptr, CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));
		std::println("Shader compilation utilities created");

		arguments = {
			DXC_ARG_DEBUG,
			DXC_ARG_SKIP_OPTIMIZATIONS,
			DXC_ARG_WARNINGS_ARE_ERRORS,
		};
		std::println("Shader compilation arguments set");
	}

	Microsoft::WRL::ComPtr<IDxcCompiler3> getCompiler() {
		return compiler;
	}

	Microsoft::WRL::ComPtr<IDxcUtils> getUtils() {
		return utils;
	}

	std::filesystem::path getShaderFolder() {
		return shaderFolder;
	}

	Microsoft::WRL::ComPtr<IDxcBlob> loadShader(LPCWSTR name, LPCWSTR type, LPCWSTR entry) {
		//std::println("Shader: {}", std::wstring(name));
		auto path = shaderFolder / name;

		uint32_t codePage = DXC_CP_ACP;
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> code;
		VERIFY_COM(utils->LoadFile(path.wstring().c_str(), &codePage, code.GetAddressOf()));
		std::println("\tFile loaded: {} bytes", code->GetBufferSize());

		Microsoft::WRL::ComPtr<IDxcCompilerArgs> args;
		VERIFY_COM(utils->BuildArguments(name, entry, type, arguments.data(), static_cast<uint32_t>(arguments.size()), nullptr, 0, args.GetAddressOf()));
		std::println("\tCompilation arguments built");

		BOOL encodingKnown = false;
		VERIFY_COM(code->GetEncoding(&encodingKnown, &codePage));

		DxcText text = {
			.Ptr = code->GetBufferPointer(),
			.Size = code->GetBufferSize(),
			.Encoding = codePage,
		};

		Microsoft::WRL::ComPtr<IDxcResult> result;
		VERIFY_COM(compiler->Compile(&text, args->GetArguments(), args->GetCount(), nullptr, IID_PPV_ARGS(result.GetAddressOf())));
		std::println("\tShader compiled");

		VERIFY_SHADER(result);
		std::println("\tResult checked");

		Microsoft::WRL::ComPtr<IDxcBlob> byteCode;
		VERIFY_COM(result->GetOutput(result->PrimaryOutput(), IID_PPV_ARGS(byteCode.GetAddressOf()), nullptr));
		std::println("\tOutput retrieved");

		return byteCode;
	}
}
