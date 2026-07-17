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

	uint32_t getCameraConstantAlignment();
	uint32_t getLightConstantAlignment();
	uint32_t getMeshConstantAlignment();
	uint32_t getCameraConstantsOffset();
	uint32_t getLightConstantsOffset();
	uint32_t getMeshConstantsOffset();
	uint32_t getConstantBufferSize();
	
	void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device);
	void recordUpload(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);

	D3D12_GPU_VIRTUAL_ADDRESS getCurrentConstantBufferView();
	void draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer);
}
