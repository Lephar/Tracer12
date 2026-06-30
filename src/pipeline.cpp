#include "pch.h"

#include "pipeline.h"
#include "shader.h"

#include "system.h"
#include "graphics.h"
#include "swapChain.h"

#include "verify.h"

namespace tracer::graphics {
	struct Pipeline::Implementation {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject;

		std::unique_ptr<Shader> vertexShader;
		std::unique_ptr<Shader> pixelShader;
	};

	Pipeline::Pipeline(const char* vertexShaderName, const char* pixelShaderName) : implementation(std::make_unique<Implementation>()) {
		D3D12_DESCRIPTOR_RANGE1 descriptorRange = {
				.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
				.NumDescriptors = 1,
				.BaseShaderRegister = 0,
				.RegisterSpace = 0,
				.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		};

		D3D12_ROOT_DESCRIPTOR_TABLE1 descriptorTable = {
			.NumDescriptorRanges = 1,
			.pDescriptorRanges = &descriptorRange,
		};

		D3D12_ROOT_PARAMETER1 rootParameter = {
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = descriptorTable,
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX,
		};

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC::Init_1_2(rootSignatureDesc, 1, &rootParameter, 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignature;
		VERIFY_COM(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, rootSignature.GetAddressOf(), nullptr));
		std::println("Versioned root signature serialized");

		auto device = getDevice();
		VERIFY_COM(device->CreateRootSignature(1, rootSignature->GetBufferPointer(), rootSignature->GetBufferSize(), IID_PPV_ARGS(implementation->rootSignature.GetAddressOf())));
		std::println("Root signature created");

		implementation->vertexShader = std::make_unique<Shader>(vertexShaderName, "vs_6_9");
		implementation->pixelShader = std::make_unique<Shader>(pixelShaderName, "ps_6_9");

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			{
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 12,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
		};

		const auto inputElementDescCount = sizeof(inputElementDescs) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {
			.pRootSignature = implementation->rootSignature.Get(),
			.VS = implementation->vertexShader->getByteCode(),
			.PS = implementation->pixelShader->getByteCode(),
			.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			.SampleMask = UINT_MAX,
			.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
			.InputLayout = {
				.pInputElementDescs = inputElementDescs,
				.NumElements = inputElementDescCount,
			},
			.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.NumRenderTargets = 1,
			.RTVFormats = {
				swapChain::getRenderTargetFormat(),
			},
			.DSVFormat = swapChain::getDepthStencilFormat(),
			.SampleDesc = {
				.Count = 1,
			},
		};

		VERIFY_COM(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(implementation->pipelineStateObject.GetAddressOf())));
		std::println("Graphics pipeline state object created");
	}

	Pipeline::~Pipeline() = default;
}
