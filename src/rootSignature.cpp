#include "pch.h"

#include "rootSignature.h"

#include "debug.h"

namespace tracer::rootSignature {
	namespace {
		Microsoft::WRL::ComPtr<ID3D12RootSignature1> rootSignature = nullptr;
	};

	void create(Microsoft::WRL::ComPtr<ID3D12Device15> device, uint32_t textureCount) {
		debug::print("Creating root signature:");
		debug::incrementDepth();

		std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;

		CD3DX12_ROOT_PARAMETER1 meshConstantBufferView;
		meshConstantBufferView.InitAsConstantBufferView(RootParameter::MeshConstantBufferView, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters.push_back(meshConstantBufferView);
		debug::print("Root parameter for mesh constant buffer view initialized at index %u", RootParameter::MeshConstantBufferView);

		CD3DX12_ROOT_PARAMETER1 cameraConstantBufferView;
		cameraConstantBufferView.InitAsConstantBufferView(RootParameter::CameraConstantBufferView, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters.push_back(cameraConstantBufferView);
		debug::print("Root parameter for camera constant buffer view initialized at index %u", RootParameter::CameraConstantBufferView);

		CD3DX12_ROOT_PARAMETER1 materialFactorConstantBufferView;
		materialFactorConstantBufferView.InitAsConstantBufferView(RootParameter::MaterialFactorConstantBufferView, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters.push_back(materialFactorConstantBufferView);
		debug::print("Root parameter for material factor constant buffer view initialized at index %u", RootParameter::MaterialFactorConstantBufferView);

		CD3DX12_ROOT_PARAMETER1 lightConstantBufferView;
		lightConstantBufferView.InitAsConstantBufferView(RootParameter::LightConstantBufferView, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters.push_back(lightConstantBufferView);
		debug::print("Root parameter for light constant buffer view initialized at index %u", RootParameter::LightConstantBufferView);
		
		CD3DX12_ROOT_PARAMETER1 constantIndexConstants;
		constantIndexConstants.InitAsConstants(4, RootParameter::ConstantIndexConstants, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters.push_back(constantIndexConstants);
		debug::print("Root parameter for constant index constants initialized at index %u", RootParameter::ConstantIndexConstants);

		CD3DX12_DESCRIPTOR_RANGE1 descriptorRange;
		descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, textureCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
		debug::print("Descriptor range set for %u material textures", textureCount);

		CD3DX12_ROOT_PARAMETER1 textureDescriptorTable;
		textureDescriptorTable.InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters.push_back(textureDescriptorTable);
		debug::print("Root parameter for texture descriptor table initialized at index %u", RootParameter::TexturesDescriptorTable);

		CD3DX12_STATIC_SAMPLER_DESC1 staticSampler;
		staticSampler.Init(0, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_SAMPLER_FLAG_NONE);
		debug::print("Static sampler created at slot 0");

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC::Init_1_2(rootSignatureDesc, static_cast<uint32_t>(rootParameters.size()), rootParameters.data(), 1, &staticSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		debug::print("Versioned root signature initialized");

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureData;
		debug::verify::com(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, rootSignatureData.GetAddressOf(), nullptr));
		debug::print("Versioned root signature serialized");

		debug::verify::com(device->CreateRootSignature(1, rootSignatureData->GetBufferPointer(), rootSignatureData->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf())));
		debug::print("Root signature created");

		debug::decrementDepth();
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> getRootSignature() {
		return rootSignature;
	}

	void bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		commandList->SetGraphicsRootSignature(rootSignature.Get());
	}
}
