#include "pch.h"

#include "pipeline.h"

#include "verify.h"

namespace tracer::graphics {
	struct Pipeline::Implementation {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	Pipeline::Pipeline(Microsoft::WRL::ComPtr<ID3D12Device15> device, Microsoft::WRL::ComPtr<IDxcBlob> vertexShader, Microsoft::WRL::ComPtr<IDxcBlob> pixelShader, DXGI_FORMAT depthStencilFormat, DXGI_FORMAT renderTargetFormat) : implementation(std::make_unique<Implementation>()) {
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

		VERIFY_COM(device->CreateRootSignature(1, rootSignature->GetBufferPointer(), rootSignature->GetBufferSize(), IID_PPV_ARGS(implementation->rootSignature.GetAddressOf())));
		std::println("Root signature created");

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			{
				.SemanticName = "TANGENT",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = sizeof(DirectX::SimpleMath::Vector4),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			{
				.SemanticName = "NORMAL",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 2 * sizeof(DirectX::SimpleMath::Vector4),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			{
				.SemanticName = "TEXCOORD",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 3 * sizeof(DirectX::SimpleMath::Vector4),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
		};

		const auto inputElementDescCount = sizeof(inputElementDescs) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {
			.pRootSignature = implementation->rootSignature.Get(),
			.VS = {
				.pShaderBytecode = vertexShader->GetBufferPointer(),
				.BytecodeLength = vertexShader->GetBufferSize(),
			},
			.PS = {
				.pShaderBytecode = pixelShader->GetBufferPointer(),
				.BytecodeLength = pixelShader->GetBufferSize(),
			},
			.BlendState = DirectX::CommonStates::AlphaBlend,
			.SampleMask = UINT_MAX,
			.RasterizerState = DirectX::CommonStates::Wireframe,
			.DepthStencilState = DirectX::CommonStates::DepthReverseZ,
			.InputLayout = {
				.pInputElementDescs = inputElementDescs,
				.NumElements = inputElementDescCount,
			},
			.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.NumRenderTargets = 1,
			.RTVFormats = {
				renderTargetFormat,
			},
			.DSVFormat = depthStencilFormat,
			.SampleDesc = {
				.Count = 1,
			},
		};

		VERIFY_COM(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(implementation->pipelineState.GetAddressOf())));
		std::println("Graphics pipeline state object created");
	}
	/*
	void Pipeline::bind() {
		auto commandList = getCommandList();

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootSignature(implementation->rootSignature.Get());
		commandList->SetPipelineState(implementation->pipelineState.Get());
	}
	*/
	Pipeline::~Pipeline() = default;
}
