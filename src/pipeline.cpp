#include "pch.h"
#include "pipeline.h"
#include "device.h"
#include "swapChain.h"
#include "rootSignature.h"
#include "debug.h"

namespace tracer {
	struct Pipeline::Implementation {
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	Pipeline::Pipeline(Microsoft::WRL::ComPtr<IDxcBlob> vertexShader, Microsoft::WRL::ComPtr<IDxcBlob> pixelShader, bool blending, bool culling) : implementation(std::make_unique<Implementation>()) {
		debug::print("Creating pipeline:");
		debug::incrementDepth();

		const auto device = device::getDevice();
		const auto sampleCount = swapChain::getSampleCount();
		const auto depthStencilFormat = swapChain::getDepthStencilFormat();
		const auto renderTargetFormat = swapChain::getRenderTargetFormat();
		const auto rootSignature = rootSignature::getRootSignature();

		debug::print("Blending %s", blending ? "enabled" : "disable");
		debug::print("Culling %s", culling ? "enabled" : "disable");

		std::vector<const char*> inputElementSemanticNames{
			"POSITION",
			"TANGENT",
			"NORMAL",
			"TEXCOORD",
		};

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements{
			inputElementSemanticNames.size(),
			D3D12_INPUT_ELEMENT_DESC {
				.SemanticName = nullptr,
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = UINT32_MAX,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			}
		};

		for (uint32_t inputElementIndex = 0; inputElementIndex < inputElements.size(); inputElementIndex++) {
			auto& inputElement = inputElements.at(inputElementIndex);

			inputElement.SemanticName = inputElementSemanticNames.at(inputElementIndex);
			inputElement.AlignedByteOffset = inputElementIndex * sizeof(DirectX::SimpleMath::Vector4);
		}

		debug::print("Input layout set");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{
			.pRootSignature = rootSignature.Get(),
			.VS = {
				.pShaderBytecode = vertexShader->GetBufferPointer(),
				.BytecodeLength = vertexShader->GetBufferSize(),
			},
			.PS = {
				.pShaderBytecode = pixelShader->GetBufferPointer(),
				.BytecodeLength = pixelShader->GetBufferSize(),
			},
			.BlendState = blending ? DirectX::CommonStates::NonPremultiplied : DirectX::CommonStates::Opaque,
			.SampleMask = UINT32_MAX,
			.RasterizerState = culling ? DirectX::CommonStates::CullClockwise : DirectX::CommonStates::CullNone,
			.DepthStencilState = DirectX::CommonStates::DepthReverseZ,
			.InputLayout = {
				.pInputElementDescs = inputElements.data(),
				.NumElements = static_cast<uint32_t>(inputElements.size()),
			},
			.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.NumRenderTargets = 1,
			.RTVFormats = {
				renderTargetFormat,
			},
			.DSVFormat = depthStencilFormat,
			.SampleDesc = {
				.Count = sampleCount,
				.Quality = DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN,
			},
		};

		debug::verify::com(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(implementation->pipelineState.GetAddressOf())));
		debug::print("Graphics pipeline state created");

		debug::decrementDepth();
	}

	Pipeline::Pipeline(Pipeline&& pipeline) noexcept : implementation(std::move(pipeline.implementation)) {}

	Pipeline& Pipeline::operator=(Pipeline&& pipeline) noexcept {
		implementation = std::move(pipeline.implementation);
		return *this;
	}

	void Pipeline::bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		commandList->SetPipelineState(implementation->pipelineState.Get());
	}

	Pipeline::~Pipeline() = default;
}
