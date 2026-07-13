#pragma once

#include "pch.h"

#include "primitive.h"
#include "mesh.h"
#include "light.h"
#include "camera.h"
#include "node.h"
#include "texture.h"
#include "material.h"
#include "asset.h"

namespace tracer::content {
	void load(std::filesystem::path dataFolder);

	std::filesystem::path getAssetFolder();

	std::vector<Asset>& getAssets();
	std::vector<Material>& getMaterials();
	std::vector<Texture>& getTextures();
	std::vector<Camera>& getCameras();
	std::vector<Light>& getLights();
	std::vector<Mesh>& getMeshes();
	std::vector<Primitive>& getPrimitives();

	std::vector<Camera::Constant>& getCameraConstants();
	std::vector<Light::Constant>& getLightConstants();
	std::vector<Mesh::Constant>& getMeshConstants();

	std::vector<Primitive::Index>& getIndices();
	std::vector<Primitive::Vertex>& getVertices();

	const uint32_t getMaterialTextureCount();

	void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_HEAP_PROPERTIES defaultHeapProperties, D3D12_HEAP_PROPERTIES uploadHeapProperties, std::shared_ptr<DirectX::DescriptorHeap> shaderResourceDescriptorHeap);
	void recordUpload(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);
}
