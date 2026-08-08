#include "pch.h"
#include "swapChain.h"
#include "frameBuffer.h"
#include "system.h"
#include "infrastructure.h"
#include "device.h"
#include "queue.h"
#include "content.h"
#include "numerics.h"
#include "debug.h"

namespace tracer::swapChain {
	namespace {
		uint32_t imageCount = 0;
		uint32_t sampleCount = 0;

		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_UNKNOWN;

		D3D12_VIEWPORT viewport = {};
		D3D12_RECT scissor = {};

		DirectX::SimpleMath::Matrix projection;

		std::unique_ptr<DirectX::DescriptorHeap> depthStencilDescriptorHeap = nullptr;
		std::unique_ptr<DirectX::DescriptorHeap> renderTargetDescriptorHeap = nullptr;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Heap1> textureHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Heap1> bufferHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource2> depthStencilBuffer = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = {};

		std::vector<FrameBuffer> frameBuffers = {};

		uint32_t imageIndex = UINT32_MAX;
	}

	void initialize() {
		debug::print("Initializing swap chain:");
		debug::incrementDepth();

		imageCount = 3;
		sampleCount = 4;
		depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
		renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		debug::print("Swap chain properties set");

		const auto width = system::getWidth();
		const auto height = system::getHeight();
		const auto window = system::getWindow();
		const auto factory = infrastructure::getFactory();
		const auto device = device::getDevice();
		const auto queue = queue::getCommandQueue();

		viewport = {
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(width),
			.Height = static_cast<float>(height),
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f,
		};

		scissor = {
			.left = 0,
			.top = 0,
			.right = static_cast<long>(width),
			.bottom = static_cast<long>(height),
		};

		debug::print("Viewport and scissor set");

		depthStencilDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
		debug::print("Depth stencil descriptor heap created with size 1");

		renderTargetDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, imageCount);
		debug::print("Render target descriptor heap created with size %u", imageCount);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
			.Width = width,
			.Height = height,
			.Format = renderTargetFormat,
			.SampleDesc = {
				.Count = 1,
			},
			.BufferUsage = 0,
			.BufferCount = imageCount,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {
			.Windowed = true,
		};

		debug::verify::com(factory->CreateSwapChainForHwnd(queue.Get(), window, &swapChainDesc, &fullscreenDesc, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())));
		debug::print("Swap chain created with %u sampled %u images", sampleCount, imageCount);
		
		frameBuffers.reserve(imageCount);
		
		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			debug::print("Frame buffer %u:", imageIndex);
			debug::incrementDepth();

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			debug::verify::com(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
			debug::print("Command allocator created");

			Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
			debug::verify::com(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
			debug::print("Fence created");

			frameBuffers.emplace_back(commandAllocator, fence);
			debug::decrementDepth();
		}

		debug::decrementDepth();
	}

	uint32_t getImageCount() {
		return imageCount;
	}

	uint32_t getSampleCount() {
		return sampleCount;
	}

	DXGI_FORMAT getDepthStencilFormat() {
		return depthStencilFormat;
	}

	DXGI_FORMAT getRenderTargetFormat() {
		return renderTargetFormat;
	}

	void createResources() {
		debug::print("Creating swap chain resources:");
		debug::incrementDepth();

		const auto width = system::getWidth();
		const auto height = system::getHeight();
		const auto device = device::getDevice();

		std::vector<D3D12_RESOURCE_DESC1> bufferResourceDescs{ imageCount, CD3DX12_RESOURCE_DESC1::Buffer(content::getConstantBufferSize()) };
		std::vector<D3D12_RESOURCE_DESC1> textureResourceDescs{ imageCount, CD3DX12_RESOURCE_DESC1::Tex2D(renderTargetFormat, width, height, 1, 1, sampleCount, DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) };
		textureResourceDescs.push_back(CD3DX12_RESOURCE_DESC1::Tex2D(depthStencilFormat, width, height, 1, 1, sampleCount, DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));

		std::vector<D3D12_RESOURCE_ALLOCATION_INFO1> bufferResourceAllocationInfos{ bufferResourceDescs.size() };
		std::vector<D3D12_RESOURCE_ALLOCATION_INFO1> textureResourceAllocationInfos{ textureResourceDescs.size() };

		device->GetResourceAllocationInfo3(1, static_cast<uint32_t>(bufferResourceDescs.size()), bufferResourceDescs.data(), 0, nullptr, bufferResourceAllocationInfos.data());
		device->GetResourceAllocationInfo3(1, static_cast<uint32_t>(textureResourceDescs.size()), textureResourceDescs.data(), 0, nullptr, textureResourceAllocationInfos.data());

		const auto& bufferAllocation = bufferResourceAllocationInfos.back();
		const auto& depthStencilAllocation = textureResourceAllocationInfos.back();

		const auto bufferSize = bufferAllocation.Offset + align(bufferAllocation.SizeInBytes, bufferAllocation.Alignment);
		const auto textureSize = depthStencilAllocation.Offset + align(depthStencilAllocation.SizeInBytes, depthStencilAllocation.Alignment);

		CD3DX12_HEAP_DESC bufferHeapDesc(bufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
		CD3DX12_HEAP_DESC textureHeapDesc(textureSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT, D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

		debug::verify::com(device->CreateHeap1(&bufferHeapDesc, nullptr, IID_PPV_ARGS(bufferHeap.GetAddressOf())));
		debug::print("Swap chain buffer resource heap created with size %lu", bufferSize);

		debug::verify::com(device->CreateHeap1(&textureHeapDesc, nullptr, IID_PPV_ARGS(textureHeap.GetAddressOf())));
		debug::print("Swap chain texture resource heap created with size %lu", textureSize);

		D3D12_CLEAR_VALUE depthStencilClearValue = {
			.Format = depthStencilFormat,
			.DepthStencil = {
				.Depth = 0.0f,
				.Stencil = 0,
			},
		};

		D3D12_CLEAR_VALUE renderTargetClearValue = {
			.Format = renderTargetFormat,
			.Color = {
				0.0f,
				0.0f,
				0.0f,
				1.0f
			},
		};

		debug::verify::com(device->CreatePlacedResource1(textureHeap.Get(), depthStencilAllocation.Offset, &textureResourceDescs.back(), D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue, IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));
		debug::print("Depth stencil buffer created on swap chain texture resource heap with offset of %lu", depthStencilAllocation.Offset);

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
			.Format = depthStencilFormat,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS,
			.Flags = D3D12_DSV_FLAG_NONE,
		};

		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {
			.Format = renderTargetFormat,
			.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS,
		};

		depthStencilView = depthStencilDescriptorHeap->GetFirstCpuHandle();
		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView);
		debug::print("Depth stencil view created");

		for (uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++) {
			debug::print("Frame buffer %u:", imageIndex);
			debug::incrementDepth();

			auto& frameBuffer = frameBuffers.at(imageIndex);

			Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer;
			const auto& renderTargetAllocation = textureResourceAllocationInfos.at(imageIndex);
			debug::verify::com(device->CreatePlacedResource1(textureHeap.Get(), renderTargetAllocation.Offset, &textureResourceDescs.at(imageIndex), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, &renderTargetClearValue, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf())));
			debug::print("Render target buffer created on swap chain texture resource heap with offset of %lu", renderTargetAllocation.Offset);

			Microsoft::WRL::ComPtr<ID3D12Resource2> resolveBuffer;
			debug::verify::com(swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(resolveBuffer.GetAddressOf())));
			debug::print("Swap chain buffer acquired");

			Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer;
			const auto& constantBufferAllocation = bufferResourceAllocationInfos.at(imageIndex);
			debug::verify::com(device->CreatePlacedResource1(bufferHeap.Get(), constantBufferAllocation.Offset, &bufferResourceDescs.at(imageIndex), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constantBuffer.GetAddressOf())));
			debug::print("Constant buffer created on swap chain buffer resource heap with offset of %lu", constantBufferAllocation.Offset);

			auto renderTargetView = renderTargetDescriptorHeap->GetCpuHandle(imageIndex);
			device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetViewDesc, renderTargetView);
			debug::print("Render target view created");

			frameBuffer.setResources(renderTargetBuffer, resolveBuffer, constantBuffer, renderTargetView);
			debug::decrementDepth();
		}

		debug::decrementDepth();
	}
	
	void begin() {
		imageIndex = swapChain->GetCurrentBackBufferIndex();
		auto& frameBuffer = frameBuffers.at(imageIndex);

		const auto commandList = queue::getCommandList();
		const auto fenceEvent = queue::getFenceEvent();

		frameBuffer.wait();
		frameBuffer.begin(depthStencilView);

		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> getCurrentConstantBuffer() {
		return frameBuffers.at(imageIndex).getConstantBuffer();
	}

	void end() {
		frameBuffers.at(imageIndex).end();
	}

	void present() {
		frameBuffers.at(imageIndex).signal();

		DXGI_PRESENT_PARAMETERS presentParameters = {};
		debug::verify::com(swapChain->Present1(0, 0, &presentParameters));
	}

	void destroy() {
		debug::print("Main loop finished");

		const auto commandQueue = queue::getCommandQueue();
		const auto commandList = queue::getCommandList();
		const auto fenceEvent = queue::getFenceEvent();

		for (auto& image : frameBuffers) {
			image.signal();
		}
		for (auto& image : frameBuffers) {
			image.wait();
		}
	}
}
