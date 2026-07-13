#include "pch.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	namespace {
		std::filesystem::path assetFolder = {};

		std::vector<Asset> assets = {};
		std::vector<Material> materials = {};
		std::vector<Texture> textures = {};
		std::vector<Camera> cameras = {};
		std::vector<Light> lights = {};
		std::vector<Mesh> meshes = {};
		std::vector<Primitive> primitives = {};

		std::vector<Camera::Constant> cameraConstants = {};
		std::vector<Light::Constant> lightConstants = {};
		std::vector<Mesh::Constant> meshConstants = {};

		std::vector<Primitive::Index> indices = {};
		std::vector<Primitive::Vertex> vertices = {};

		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingIndexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingVertexBuffer = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource2> indexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> vertexBuffer = nullptr;

		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	}

	void load(std::filesystem::path dataFolder) {
		debug::print("Loading content:");
		debug::incrementDepth();

		assetFolder = dataFolder / "assets";
		debug::print("Asset folder set: %s", assetFolder.string().c_str());

		assets.emplace_back("Sponza", "Sponza_Main.gltf");

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
	
	std::vector<Camera>& getCameras() {
		return cameras;
	}
	
	std::vector<Light>& getLights() {
		return lights;
	}
	
	std::vector<Mesh>& getMeshes() {
		return meshes;
	}
	
	std::vector<Primitive>& getPrimitives() {
		return primitives;
	}
	
	std::vector<Camera::Constant>& getCameraConstants() {
		return cameraConstants;
	}
	
	std::vector<Light::Constant>& getLightConstants() {
		return lightConstants;
	}
	
	std::vector<Mesh::Constant>& getMeshConstants() {
		return meshConstants;
	}
	
	std::vector<Primitive::Index>& getIndices() {
		return indices;
	}
	
	std::vector<Primitive::Vertex>& getVertices() {
		return vertices;
	}

	const uint32_t getMaterialTextureCount() {
		return Material::getTextureCount();
	}

	void createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_HEAP_PROPERTIES defaultHeapProperties, D3D12_HEAP_PROPERTIES uploadHeapProperties, std::shared_ptr<DirectX::DescriptorHeap> shaderResourceDescriptorHeap) {
		debug::print("Creating content resources:");
		debug::incrementDepth();
		
		for (uint32_t textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
			debug::print("Texture: %u", textureIndex);
			debug::incrementDepth();

			auto& texture = textures.at(textureIndex);
			auto shaderResourceView = shaderResourceDescriptorHeap->GetCpuHandle(textureIndex);
			texture.createResources(device, uploadHeapProperties, shaderResourceView);
			
			debug::decrementDepth();
		}

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
	/*
	namespace {
		
		Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer = nullptr;
		void* constantBufferMemory = nullptr;
	}

	void loadAAAAAAAAAAAA() {
		auto device = getDevice();

		auto commandQueue = getCommandQueue();
		auto commandList = getCommandList();

		auto defaultHeapProperties = memory::getDefaultHeapProperties();
		auto uploadHeapProperties = memory::getUploadHeapProperties();

		assets.emplace_back("Sponza", "Sponza_Main");

		
		const auto swapChainImageCount = swapChain::getImageCount();
		constantBufferAlignment = static_cast<uint32_t>(align(sizeof(Constant), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		const auto constantBufferSize = static_cast<uint32_t>(align(swapChainImageCount * constantBufferAlignment, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT));

		auto constantBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(constantBufferSize);
		debug::verify::com(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &constantBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(constantBuffer.GetAddressOf())));
		debug::print("Constant buffer created");

		CD3DX12_RANGE readRange(0, 0);
		debug::verify::com(constantBuffer->Map(0, &readRange, &constantBufferMemory));
		debug::print("Constant buffer memory mapped");

	}

	uint32_t getConstantBufferAlignment() {
		return constantBufferAlignment;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> getConstantBuffer() {
		return constantBuffer;
	}

	void* getConstantBufferMemory() {
		return constantBufferMemory;
	}

	void update() {
		auto width = system::getWidth();
		auto height = system::getHeight();
		auto timeDelta = system::getTimeDelta();
		auto mouseState = system::getMouseState();
		auto keyboardState = system::getKeyboardState();

		forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, -2.0f * mouseState.x / width));

		DirectX::SimpleMath::Vector3 right = forward.Cross(up);
		forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(right, -2.0f * mouseState.y / height));

		DirectX::SimpleMath::Vector3 movement {
			timeDelta * (keyboardState.D - keyboardState.A),
			timeDelta * (keyboardState.R - keyboardState.F),
			timeDelta * (keyboardState.W - keyboardState.S),
		};

		position += movement.x * right;
		position += movement.y * up;
		position += movement.z * forward;

		constants.view = DirectX::SimpleMath::Matrix::CreateLookAt(position, position + forward, DirectX::SimpleMath::Vector3::UnitY);
	}

	void draw() {
		auto commandList = getCommandList();
		const uint32_t indexCount = indexBufferView.SizeInBytes / sizeof(Index);

		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

		for (auto& asset : assets) {
			asset.draw();
		}
	}
	*/
}
