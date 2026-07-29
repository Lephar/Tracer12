#include "pch.h"

#include "pipeline.h"

#include "debug.h"

namespace tracer::graphics {
	struct Pipeline::Implementation {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	Pipeline::Pipeline(Microsoft::WRL::ComPtr<ID3D12Device15> device, Microsoft::WRL::ComPtr<IDxcBlob> vertexShader, Microsoft::WRL::ComPtr<IDxcBlob> pixelShader, DXGI_FORMAT depthStencilFormat, DXGI_FORMAT renderTargetFormat, uint32_t textureCount) : implementation(std::make_unique<Implementation>()) {
		debug::print("Creating pipeline:");
		debug::incrementDepth();
		
		CD3DX12_DESCRIPTOR_RANGE1 descriptorRange;
		descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, textureCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

		CD3DX12_ROOT_PARAMETER1 meshConstantBufferView;
		CD3DX12_ROOT_PARAMETER1 cameraConstantBufferView;
		CD3DX12_ROOT_PARAMETER1 materialConstantBufferView;
		CD3DX12_ROOT_PARAMETER1 lightConstantBufferView;
		CD3DX12_ROOT_PARAMETER1 constants;
		CD3DX12_ROOT_PARAMETER1 textures;

		meshConstantBufferView.InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
		cameraConstantBufferView.InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		materialConstantBufferView.InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
		lightConstantBufferView.InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
		constants.InitAsConstants(4, 4, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		textures.InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

		std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;

		rootParameters.push_back(meshConstantBufferView);
		rootParameters.push_back(cameraConstantBufferView);
		rootParameters.push_back(materialConstantBufferView);
		rootParameters.push_back(lightConstantBufferView);
		rootParameters.push_back(constants);
		rootParameters.push_back(textures);

		CD3DX12_STATIC_SAMPLER_DESC1 staticSampler;
		staticSampler.Init(0, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_SAMPLER_FLAG_NONE);

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC::Init_1_2(rootSignatureDesc, static_cast<uint32_t>(rootParameters.size()), rootParameters.data(), 1, &staticSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignature;
		debug::verify::com(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, rootSignature.GetAddressOf(), nullptr));
		debug::print("Versioned root signature serialized");

		debug::verify::com(device->CreateRootSignature(1, rootSignature->GetBufferPointer(), rootSignature->GetBufferSize(), IID_PPV_ARGS(implementation->rootSignature.GetAddressOf())));
		debug::print("Root signature created");

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
		
		auto blendState = DirectX::CommonStates::AlphaBlend;
		auto& blendDesc = blendState.RenderTarget[0];

		blendDesc.BlendEnable = true;
		blendDesc.LogicOpEnable = false;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

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
			.BlendState = blendState,
			.SampleMask = UINT32_MAX,
			.RasterizerState = DirectX::CommonStates::CullClockwise,
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

		debug::verify::com(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(implementation->pipelineState.GetAddressOf())));
		debug::print("Graphics pipeline state object created");

		debug::decrementDepth();
	}
	
	void Pipeline::bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootSignature(implementation->rootSignature.Get());
		commandList->SetPipelineState(implementation->pipelineState.Get());
	}
	
	Pipeline::~Pipeline() = default;
}
