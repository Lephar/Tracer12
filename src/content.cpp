#include "pch.h"

#include "content.h"

#include "system.h"
#include "graphics.h"
#include "memory.h"
#include "swapChain.h"

#include "verify.h"
#include "numerics.h"

namespace tracer::graphics::content {
	namespace {
		const Index indices[] = {
			0, 2, 1,
			0, 1, 3,

			4, 6, 5,
			4, 5, 7,

			8, 10, 9,
			8, 9, 11,

			12, 14, 13,
			12, 13, 15,

			16, 18, 17,
			16, 17, 19,

			20, 22, 21,
			20, 21, 23,
		};

		const Vertex vertices[] = {
			{
				{-0.5f,  0.5f, -0.5f},
				{ 1.0f,  0.0f,  0.0f},
			}, {
				{ 0.5f, -0.5f, -0.5f},
				{ 1.0f,  0.0f,  1.0f},
			}, {
				{-0.5f, -0.5f, -0.5f},
				{ 0.0f,  0.0f,  1.0f},
			}, {
				{ 0.5f,  0.5f, -0.5f},
				{ 0.0f,  1.0f,  0.0f},
			},

			{
				{ 0.5f, -0.5f, -0.5f},
				{ 1.0f,  0.0f,  0.0f},
			}, {
				{ 0.5f,  0.5f,  0.5f},
				{ 1.0f,  0.0f,  1.0f},
			}, {
				{ 0.5f, -0.5f,  0.5f},
				{ 0.0f,  0.0f,  1.0f},
			}, {
				{ 0.5f,  0.5f, -0.5f},
				{ 0.0f,  1.0f,  0.0f},
			},

			{
				{-0.5f,  0.5f,  0.5f},
				{ 1.0f,  0.0f,  0.0f},
			}, {
				{-0.5f, -0.5f, -0.5f},
				{ 1.0f,  0.0f,  1.0f},
			}, {
				{-0.5f, -0.5f,  0.5f},
				{ 0.0f,  0.0f,  1.0f},
			}, {
				{-0.5f,  0.5f, -0.5f},
				{ 0.0f,  1.0f,  0.0f},
			},

			{
				{ 0.5f,  0.5f,  0.5f},
				{ 1.0f,  0.0f,  0.0f},
			}, {
				{-0.5f, -0.5f,  0.5f},
				{ 1.0f,  0.0f,  1.0f},
			}, {
				{ 0.5f, -0.5f,  0.5f},
				{ 0.0f,  0.0f,  1.0f},
			}, {
				{-0.5f,  0.5f,  0.5f},
				{ 0.0f,  1.0f,  0.0f},
			},

			{
				{-0.5f,  0.5f, -0.5f},
				{ 1.0f,  0.0f,  0.0f},
			}, {
				{ 0.5f,  0.5f,  0.5f},
				{ 1.0f,  0.0f,  1.0f},
			}, {
				{ 0.5f,  0.5f, -0.5f},
				{ 0.0f,  0.0f,  1.0f},
			}, {
				{-0.5f,  0.5f,  0.5f},
				{ 0.0f,  1.0f,  0.0f},
			},

			{
				{ 0.5f, -0.5f,  0.5f},
				{ 1.0f,  0.0f,  0.0f},
			}, {
				{-0.5f, -0.5f, -0.5f},
				{ 1.0f,  0.0f,  1.0f},
			}, {
				{ 0.5f, -0.5f, -0.5f},
				{ 0.0f,  0.0f,  1.0f},
			}, {
				{-0.5f, -0.5f,  0.5f},
				{ 0.0f,  1.0f,  0.0f},
			},
		};

		Constant constants = {};

		DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, -4.0f };
		DirectX::SimpleMath::Vector3 forward = { 0.0f, 0.0f, 1.0f };
		const DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
		
		float fieldOfView = DirectX::XM_PIDIV4;
		float aspectRatio = 0.0f;
		const float nearPlane = 0.25f;
		const float farPlane = 256.0f;

		uint32_t constantBufferAlignment = UINT32_MAX;

		Microsoft::WRL::ComPtr<ID3D12Resource2> indexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> vertexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer = nullptr;

		void* constantBufferMemory = nullptr;

		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	}

	void load() {
		auto device = getDevice();

		auto commandQueue = getCommandQueue();
		auto commandList = getCommandList();

		auto defaultHeapProperties = memory::getDefaultHeapProperties();
		auto uploadHeapProperties = memory::getUploadHeapProperties();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		VERIFY_COM(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
		std::println("Content command allocator created");

		uint64_t fenceValue = 0;
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
		VERIFY_COM(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
		std::println("Content fence created and value set");

		const uint32_t indicesSize = sizeof(indices);
		const auto indexBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(indicesSize);
		VERIFY_COM(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &indexBufferResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, nullptr, IID_PPV_ARGS(indexBuffer.GetAddressOf())));
		std::println("Index buffer created on default heap");

		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingIndexBuffer;
		VERIFY_COM(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &indexBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(stagingIndexBuffer.GetAddressOf())));
		std::println("Staging index buffer created on upload heap");

		const uint32_t verticesSize = sizeof(vertices);
		const auto vertexBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(verticesSize);
		VERIFY_COM(device->CreateCommittedResource2(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexBufferResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, nullptr, IID_PPV_ARGS(vertexBuffer.GetAddressOf())));
		std::println("Vertex buffer created on default heap");

		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingVertexBuffer;
		VERIFY_COM(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(stagingVertexBuffer.GetAddressOf())));
		std::println("Staging vertex buffer created on upload heap");

		const auto swapChainImageCount = swapChain::getImageCount();
		constantBufferAlignment = static_cast<uint32_t>(align(sizeof(Constant), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		const auto constantBufferSize = static_cast<uint32_t>(align(swapChainImageCount * constantBufferAlignment, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT));

		auto constantBufferResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(constantBufferSize);
		VERIFY_COM(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &constantBufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(constantBuffer.GetAddressOf())));
		std::println("Constant buffer created");

		CD3DX12_RANGE readRange(0, 0);
		VERIFY_COM(constantBuffer->Map(0, &readRange, &constantBufferMemory));
		std::println("Constant buffer memory mapped");

		VERIFY_COM(commandAllocator->Reset());
		VERIFY_COM(commandList->Reset(commandAllocator.Get(), nullptr));
		std::println("Content command allocator and list reset");

		D3D12_SUBRESOURCE_DATA indexSubresourceData = {
			.pData = indices,
			.RowPitch = indicesSize,
			.SlicePitch = indicesSize,
		};

		D3D12_SUBRESOURCE_DATA vertexSubresourceData = {
			.pData = vertices,
			.RowPitch = verticesSize,
			.SlicePitch = verticesSize,
		};

		UpdateSubresources(commandList.Get(), indexBuffer.Get(), stagingIndexBuffer.Get(), 0, 0, 1, &indexSubresourceData);
		UpdateSubresources(commandList.Get(), vertexBuffer.Get(), stagingVertexBuffer.Get(), 0, 0, 1, &vertexSubresourceData);

		const CD3DX12_RESOURCE_BARRIER bufferBarriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER),
			CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
		};

		const auto bufferBarrierCount = sizeof(bufferBarriers) / sizeof(CD3DX12_RESOURCE_BARRIER);

		commandList->ResourceBarrier(bufferBarrierCount, bufferBarriers);

		VERIFY_COM(commandList->Close());

		commandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(commandList.GetAddressOf()));
		std::println("Subresource update command submitted");

		fenceValue++;
		VERIFY_COM(commandQueue->Signal(fence.Get(), fenceValue));

		if (fence->GetCompletedValue() < fenceValue) {
			auto fenceEvent = system::getEvent();

			VERIFY_COM(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			VERIFY_COM(WaitForSingleObject(fenceEvent, INFINITE));
		}

		std::println("Subresource update command completed");

		indexBufferView = {
			.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = indicesSize,
			.Format = DXGI_FORMAT_R32_UINT,
		};

		std::println("Index buffer view set");

		vertexBufferView = {
			.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = verticesSize,
			.StrideInBytes = sizeof(Vertex),
		};

		std::println("Vertex buffer view set");

		aspectRatio = static_cast<float>(system::getWidth()) / static_cast<float>(system::getHeight());
		constants.projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);;

		std::println("Projection matrix generated");
	}

	Constant& getConstants() {
		return constants;
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
		commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}
}
