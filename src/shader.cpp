#include "pch.h"

#include "shader.h"

#include "system.h"
#include "graphics.h"

#include "verify.h"

namespace tracer::graphics {
	struct Shader::Implementation {
		Microsoft::WRL::ComPtr<IDxcBlob> byteCode;
	};

	Shader::Shader(LPCWSTR name, LPCWSTR type) : implementation(std::make_unique<Implementation>()) {
		auto path = system::getDataFolder() / "shaders" / (std::wstring(name) + L".hlsl");
		std::println("Shader path: {}", path.string());

		BOOL encodingKnown = false;
		uint32_t codePage = DXC_CP_ACP;
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> code;

		auto compilerUtils = getCompilerUtils();
		VERIFY_COM(compilerUtils->LoadFile(path.wstring().c_str(), &codePage, code.GetAddressOf()));
		std::println("\tFile loaded: {} bytes", code->GetBufferSize());

		VERIFY_COM(code->GetEncoding(&encodingKnown, &codePage));

		DxcText text = {
			.Ptr = code->GetBufferPointer(),
			.Size = code->GetBufferSize(),
			.Encoding = codePage,
		};

		LPCWSTR argumentList[] = {
			DXC_ARG_DEBUG,
			DXC_ARG_SKIP_OPTIMIZATIONS,
			DXC_ARG_WARNINGS_ARE_ERRORS,
		};

		const uint32_t argumentCount = sizeof(argumentList) / sizeof(LPCWSTR);

		Microsoft::WRL::ComPtr<IDxcCompilerArgs> arguments;
		VERIFY_COM(compilerUtils->BuildArguments(name, L"main", type, argumentList, argumentCount, nullptr, 0, arguments.GetAddressOf()));
		std::println("\tArguments built");

		Microsoft::WRL::ComPtr<IDxcResult> result;
		VERIFY_COM(getCompiler()->Compile(&text, arguments->GetArguments(), arguments->GetCount(), nullptr, IID_PPV_ARGS(result.GetAddressOf())));
		std::println("\tShader compiled");

		VERIFY_SHADER(result);
		std::println("\tResult checked");

		VERIFY_COM(result->GetOutput(result->PrimaryOutput(), IID_PPV_ARGS(implementation->byteCode.GetAddressOf()), nullptr));
		std::println("\tOutput retrieved");
	}

	D3D12_SHADER_BYTECODE Shader::getByteCode() {
		return D3D12_SHADER_BYTECODE{
			.pShaderBytecode = implementation->byteCode->GetBufferPointer(),
			.BytecodeLength = implementation->byteCode->GetBufferSize(),
		};
	}

	Shader::~Shader() = default;
}
