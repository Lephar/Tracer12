#include "pch.h"

#include "content.h"

#include "debug.h"
#include "numerics.h"

namespace tracer::content {
	namespace {
		std::filesystem::path assetFolder = {};

		std::vector<Asset> assets = {};
		std::vector<Material> materials = {};
		std::vector<Texture> textures = {};
		std::vector<Mesh> meshes = {};
		std::vector<Camera> cameras = {};
		std::vector<Light> lights = {};
		std::vector<Primitive> primitives = {};

		std::vector<Mesh::Constant> meshConstants = {};
		std::vector<Camera::Constant> cameraConstants = {};
		std::vector<Light::Constant> lightConstants = {};

		std::vector<Primitive::Index> indices = {};
		std::vector<Primitive::Vertex> vertices = {};

		std::unique_ptr<DirectX::DescriptorHeap> shaderResourceDescriptorHeap = nullptr;

		uint32_t defaultCameraIndex = UINT32_MAX;

		uint32_t meshConstantAlignment = UINT32_MAX;
		uint32_t cameraConstantAlignment = UINT32_MAX;
		uint32_t lightConstantAlignment = UINT32_MAX;

		uint32_t meshConstantsSize = UINT32_MAX;
		uint32_t cameraConstantsSize = UINT32_MAX;
		uint32_t lightConstantsSize = UINT32_MAX;

		uint32_t meshConstantsOffset = UINT32_MAX;
		uint32_t cameraConstantsOffset = UINT32_MAX;
		uint32_t lightConstantsOffset = UINT32_MAX;

		uint32_t constantBufferSize = UINT32_MAX;

		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingIndexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingVertexBuffer = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource2> indexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> vertexBuffer = nullptr;

		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		D3D12_GPU_VIRTUAL_ADDRESS currentConstantBufferView = {};

		DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::UnitZ;
		DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
	}

	void load(std::filesystem::path dataFolder, float aspectRatio) {
		debug::print("Loading content:");
		debug::incrementDepth();

		assetFolder = dataFolder / "assets";
		debug::print("Asset folder set: %s", assetFolder.string().c_str());

		materials.resize(1);
		assets.emplace_back("Sponza", "Sponza_Main.gltf");
		
		for (auto& camera : cameras) {
			camera.adjust(aspectRatio);
		}

		defaultCameraIndex = 0;

		meshConstantsOffset = 0;
		meshConstantAlignment = static_cast<uint32_t>(align(sizeof(Mesh::Constant), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		meshConstantsSize = static_cast<uint32_t>(meshConstants.size() * meshConstantAlignment);

		cameraConstantsOffset = meshConstantsOffset + meshConstantsSize;
		cameraConstantAlignment = static_cast<uint32_t>(align(sizeof(Camera::Constant), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		cameraConstantsSize = static_cast<uint32_t>(cameraConstants.size() * cameraConstantAlignment);

		lightConstantsOffset = cameraConstantsOffset + cameraConstantsSize;
		lightConstantAlignment = static_cast<uint32_t>(sizeof(Light::Constant));
		lightConstantsSize = static_cast<uint32_t>(align(lightConstants.size() * lightConstantAlignment, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

		constantBufferSize = static_cast<uint32_t>(align(lightConstantsOffset + lightConstantsSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT));

		debug::decrementDepth();
	}

	std::filesystem::path getAssetFolder() {
		return assetFolder;
	}

	std::vector<Asset>& getAssets() {
		return assets;
	}
	
	std::vector<Material>& getMaterials() {
		return materials;
	}
	
	std::vector<Texture>& getTextures() {
		return textures;
	}

	std::vector<Mesh>& getMeshes() {
		return meshes;
	}
	
	std::vector<Camera>& getCameras() {
		return cameras;
	}
	
	std::vector<Light>& getLights() {
		return lights;
	}
	
	std::vector<Primitive>& getPrimitives() {
		return primitives;
	}

	std::vector<Mesh::Constant>& getMeshConstants() {
		return meshConstants;
	}
	
	std::vector<Camera::Constant>& getCameraConstants() {
		return cameraConstants;
	}
	
	std::vector<Light::Constant>& getLightConstants() {
		return lightConstants;
	}
	
	std::vector<Primitive::Index>& getIndices() {
		return indices;
	}
	
	std::vector<Primitive::Vertex>& getVertices() {
		return vertices;
	}

	uint32_t getMeshConstantAlignment() {
		return meshConstantAlignment;
	}

	uint32_t getCameraConstantAlignment() {
		return cameraConstantAlignment;
	}

	uint32_t getLightConstantAlignment() {
		return lightConstantAlignment;
	}

	uint32_t getMeshConstantsOffset() {
		return meshConstantsOffset;
	}

	uint32_t getCameraConstantsOffset() {
		return cameraConstantsOffset;
	}

	uint32_t getLightConstantsOffset() {
		return lightConstantsOffset;
	}

	uint32_t getConstantBufferSize() {
		return constantBufferSize;
	}

	void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device) {
		debug::print("Creating content resources:");
		debug::incrementDepth();

		const auto textureCount = static_cast<uint32_t>(textures.size());
		shaderResourceDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, textureCount);
		debug::print("Shader resource descriptor heap created with size %u", textureCount);
		
		for (uint32_t textureIndex = 0; textureIndex < textureCount; textureIndex++) {
			debug::print("Texture: %u", textureIndex);
			debug::incrementDepth();

			auto& texture = textures.at(textureIndex);
			auto shaderResourceView = shaderResourceDescriptorHeap->GetCpuHandle(textureIndex);
			texture.createResources(device, shaderResourceView);
			
			debug::decrementDepth();
		}

		const auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto indicesSize = static_cast<uint32_t>(indices.size() * sizeof(Primitive::Index));
		auto verticesSize = static_cast<uint32_t>(vertices.size() * sizeof(Primitive::Vertex));

		const auto indexBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(indicesSize);
		const auto vertexBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(verticesSize);
		
		debug::verify::com(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &indexBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(stagingIndexBuffer.GetAddressOf())));
		debug::print("Staging index buffer created on upload heap");

		debug::verify::com(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(stagingVertexBuffer.GetAddressOf())));
		debug::print("Staging vertex buffer created on upload heap");

		debug::verify::com(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &indexBufferResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, nullptr, IID_PPV_ARGS(indexBuffer.GetAddressOf())));
		debug::print("Index buffer created on default heap");

		debug::verify::com(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexBufferResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, nullptr, IID_PPV_ARGS(vertexBuffer.GetAddressOf())));
		debug::print("Vertex buffer created on default heap");

		indexBufferView = {
			.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = indicesSize,
			.Format = DXGI_FORMAT_R32_UINT,
		};

		debug::print("Index buffer view set");

		vertexBufferView = {
			.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = verticesSize,
			.StrideInBytes = sizeof(Primitive::Vertex),
		};

		debug::print("Vertex buffer view set");

		debug::decrementDepth();
	}

	void recordUpload(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		debug::print("Recording content upload:");
		debug::incrementDepth();

		for (uint32_t textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
			debug::print("Texture: %u", textureIndex);
			debug::incrementDepth();

			auto& texture = textures.at(textureIndex);
			texture.recordUpload(commandList);

			debug::decrementDepth();
		}

		auto indicesSize = static_cast<uint32_t>(indices.size() * sizeof(Primitive::Index));
		auto verticesSize = static_cast<uint32_t>(vertices.size() * sizeof(Primitive::Vertex));

		D3D12_SUBRESOURCE_DATA indexSubresourceData = {
			.pData = indices.data(),
			.RowPitch = indicesSize,
			.SlicePitch = indicesSize,
		};

		D3D12_SUBRESOURCE_DATA vertexSubresourceData = {
			.pData = vertices.data(),
			.RowPitch = verticesSize,
			.SlicePitch = verticesSize,
		};

		UpdateSubresources(commandList.Get(), indexBuffer.Get(), stagingIndexBuffer.Get(), 0, 0, 1, &indexSubresourceData);
		UpdateSubresources(commandList.Get(), vertexBuffer.Get(), stagingVertexBuffer.Get(), 0, 0, 1, &vertexSubresourceData);
		debug::print("Buffer upload commands recorded");

		const D3D12_RESOURCE_BARRIER bufferBarriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER),
			CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
		};

		const auto bufferBarrierCount = sizeof(bufferBarriers) / sizeof(D3D12_RESOURCE_BARRIER);

		commandList->ResourceBarrier(bufferBarrierCount, bufferBarriers);
		debug::print("Buffer barriers recorded");

		debug::decrementDepth();
	}

	void clearStaging() {
		debug::print("Cleaning up staging resources:");
		debug::incrementDepth();

		for (uint32_t textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
			debug::print("Texture: %u", textureIndex);
			debug::incrementDepth();

			auto& texture = textures.at(textureIndex);
			texture.clearStaging();

			debug::decrementDepth();
		}

		indices.clear();
		vertices.clear();

		stagingIndexBuffer.Reset();
		stagingVertexBuffer.Reset();

		debug::print("Staging index and vertex resources cleaned up");

		debug::decrementDepth();
	}

	D3D12_GPU_VIRTUAL_ADDRESS getCurrentConstantBufferView() {
		return currentConstantBufferView;
	}
	
	void update(DirectX::SimpleMath::Vector2 mouseMovement, DirectX::SimpleMath::Vector3 keyboardMovement) {
		const auto turnSpeed = 1.0f;
		const auto moveSpeed = 4.0f;

		mouseMovement *= turnSpeed;
		keyboardMovement *= moveSpeed;

		forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, mouseMovement.x));

		DirectX::SimpleMath::Vector3 right = forward.Cross(up);
		forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(right, mouseMovement.y));

		position += keyboardMovement.x * right;
		position += keyboardMovement.y * up;
		position += keyboardMovement.z * forward;

		auto transform = DirectX::SimpleMath::Matrix::CreateTranslation(position.x, position.y, position.z);
		auto view = DirectX::SimpleMath::Matrix::CreateLookAt(position, position + forward, up);

		cameras.at(defaultCameraIndex).update(transform, view);
	}

	void draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer) {
		CD3DX12_RANGE readRange(0, 0);
		uint8_t* constantBufferMemory;
		debug::verify::com(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&constantBufferMemory)));

		for (uint32_t meshIndex = 0; meshIndex < meshConstants.size(); meshIndex++) {
			memcpy(constantBufferMemory + meshConstantsOffset + meshIndex * meshConstantAlignment, &meshConstants.at(meshIndex), sizeof(Mesh::Constant));
		}

		for (uint32_t cameraIndex = 0; cameraIndex < cameraConstants.size(); cameraIndex++) {
			memcpy(constantBufferMemory + cameraConstantsOffset + cameraIndex * cameraConstantAlignment, &cameraConstants.at(cameraIndex), sizeof(Camera::Constant));
		}

		for (uint32_t lightIndex = 0; lightIndex < lightConstants.size(); lightIndex++) {
			memcpy(constantBufferMemory + lightConstantsOffset + lightIndex * lightConstantAlignment, &lightConstants.at(lightIndex), sizeof(Light::Constant));
		}

		constantBuffer->Unmap(0, nullptr);
		currentConstantBufferView = constantBuffer->GetGPUVirtualAddress();

		const auto descriptorHeap = shaderResourceDescriptorHeap->Heap();
		const auto descriptorTable = shaderResourceDescriptorHeap->GetFirstGpuHandle();
		
		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->SetDescriptorHeaps(1, &descriptorHeap);
		commandList->SetGraphicsRootDescriptorTable(4, descriptorTable);

		auto& camera = cameras.at(defaultCameraIndex);
		camera.bind(commandList);
		Light::bindAll(commandList);

		for (auto& asset : assets) {
			asset.draw(commandList);
		}
	}
}
