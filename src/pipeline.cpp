#include "pch.h"
#include "pipeline.h"
#include "compiler.h"
#include "device.h"
#include "swapChain.h"
#include "rootSignature.h"
#include "debug.h"

namespace tracer::pipeline {
	namespace {
		std::map<std::pair<bool, bool>, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates;
	}
	
	void build() {
		debug::print("Creating pipelines:");
		debug::incrementDepth();

		const auto device = device::getDevice();
		const auto sampleCount = swapChain::getSampleCount();
		const auto depthStencilFormat = swapChain::getDepthStencilFormat();
		const auto renderTargetFormat = swapChain::getRenderTargetFormat();
		const auto rootSignature = rootSignature::getRootSignature();

		const auto vertexShader = compiler::loadShader(L"vertex.hlsl", L"vs_6_9", L"main");
		const auto pixelShader = compiler::loadShader(L"pixel.hlsl", L"ps_6_9", L"main");

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
			.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			.SampleMask = UINT32_MAX,
			.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
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

		const std::vector<bool> states{ false, true };

		for (const auto blending : states) {
			for (const auto culling : states) {
				pipelineStateDesc.BlendState = blending ? DirectX::CommonStates::NonPremultiplied : DirectX::CommonStates::Opaque;
				pipelineStateDesc.RasterizerState = culling ? DirectX::CommonStates::CullClockwise : DirectX::CommonStates::CullNone;

				Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
				debug::verify::com(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipelineState.GetAddressOf())));
				debug::print("Pipeline state created with blending %s and culling %s", blending ? "enabled" : "disabled", culling ? "enabled" : "disabled");

				pipelineStates.insert(std::make_pair(std::make_pair(blending, culling), pipelineState));
			}
		}

		debug::decrementDepth();
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> getPipelineState(bool blending, bool culling) {
		return pipelineStates.at(std::make_pair(blending, culling));
	}
}
