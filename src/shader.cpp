#include "pch.h"

#include "shader.h"

#include "system.h"
#include "graphics.h"

#include "verify.h"

namespace tracer::graphics {
	struct Shader::Implementation {
		Microsoft::WRL::ComPtr<IDxcBlob> byteCode;
	};

	Shader::Shader(const char* name, const char* type) : implementation(std::make_unique<Implementation>()) {
		std::println("Shader name: {}", name);

		auto path = system::getDataFolder() / "shaders" / (std::string(name) + ".hlsl");
		std::println("\tPath: {}", path.string());

		auto size = std::filesystem::file_size(path);
		std::println("\tFile size: {}", size);

		std::vector<char> content(size);
		std::println("\tBuffer allocated");

		std::ifstream file(path, std::ios_base::in);
		std::println("\tFile opened in read mode");

		file.read(content.data(), size);
		std::println("\tData loaded into buffer");

		size = file.gcount();
		std::println("\tData size: {}", size);

		DxcText text = {
			.Ptr = content.data(),
			.Size = size,
			.Encoding = 0,
		};

		WCHAR wName[UCHAR_MAX];
		VERIFY(swprintf_s(wName, UCHAR_MAX, L"%hs", name) != -1);

		WCHAR wType[UCHAR_MAX];
		VERIFY(swprintf_s(wType, UCHAR_MAX, L"%hs", type) != -1);

		Microsoft::WRL::ComPtr<IDxcCompilerArgs> arguments;
		VERIFY_COM(getCompilerUtils()->BuildArguments(wName, L"main", wType, nullptr, 0, nullptr, 0, arguments.GetAddressOf()));
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
