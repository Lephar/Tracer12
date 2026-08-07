#include "pch.h"
#include "compiler.h"
#include "system.h"
#include "debug.h"

namespace tracer::compiler {
	namespace {
		std::filesystem::path shaderFolder = {};

		Microsoft::WRL::ComPtr<IDxcCompiler3> compiler = nullptr;
		Microsoft::WRL::ComPtr<IDxcUtils> utils = nullptr;

		std::vector<LPCWSTR> arguments = {};
	}

	void initialize() {
		debug::print("Initializing compiler:");
		debug::incrementDepth();
		
		shaderFolder = system::getDataFolder() / "shaders";
		debug::print("Shader folder set: %s", shaderFolder.string().c_str());

		debug::verify::com(DxcCreateInstance2(nullptr, CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
		debug::print("Shader compiler created");

		debug::verify::com(DxcCreateInstance2(nullptr, CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));
		debug::print("Shader compilation utilities created");

		arguments = {
			DXC_ARG_WARNINGS_ARE_ERRORS,
		};

		if (debug::enabled) {
			arguments.push_back(DXC_ARG_DEBUG);
			arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
		}
		else {
			arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
		}

		debug::print("Shader compilation arguments set");

		debug::decrementDepth();
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
		debug::print("Shader: %S", name);
		debug::incrementDepth();
		
		auto path = shaderFolder / name;

		uint32_t codePage = DXC_CP_ACP;
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> code;
		debug::verify::com(utils->LoadFile(path.wstring().c_str(), &codePage, code.GetAddressOf()));
		debug::print("File loaded: %lu bytes", code->GetBufferSize());

		Microsoft::WRL::ComPtr<IDxcCompilerArgs> args;
		debug::verify::com(utils->BuildArguments(name, entry, type, arguments.data(), static_cast<uint32_t>(arguments.size()), nullptr, 0, args.GetAddressOf()));
		debug::print("Compilation arguments built");

		BOOL encodingKnown = false;
		debug::verify::com(code->GetEncoding(&encodingKnown, &codePage));

		DxcText text = {
			.Ptr = code->GetBufferPointer(),
			.Size = code->GetBufferSize(),
			.Encoding = codePage,
		};

		Microsoft::WRL::ComPtr<IDxcResult> result;
		debug::verify::com(compiler->Compile(&text, args->GetArguments(), args->GetCount(), nullptr, IID_PPV_ARGS(result.GetAddressOf())));
		debug::print("Shader compiled");

		debug::verify::dxc(result);
		debug::print("Result checked");

		Microsoft::WRL::ComPtr<IDxcBlob> byteCode;
		debug::verify::com(result->GetOutput(result->PrimaryOutput(), IID_PPV_ARGS(byteCode.GetAddressOf()), nullptr));
		debug::print("Output retrieved");

		debug::decrementDepth();

		return byteCode;
	}
}
