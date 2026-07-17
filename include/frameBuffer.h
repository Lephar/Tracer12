#pragma once

namespace tracer::graphics {
	class FrameBuffer {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		struct Constant {
			DirectX::SimpleMath::Matrix projection;
		};

		FrameBuffer(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, Microsoft::WRL::ComPtr<ID3D12Fence1> fence);
		
		FrameBuffer(const FrameBuffer& image) = delete;
		FrameBuffer& operator=(const FrameBuffer& image) = delete;

		FrameBuffer(FrameBuffer&& image) noexcept;
		FrameBuffer& operator=(FrameBuffer&& image) noexcept;

		void setResources(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer);
		
		void wait(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, HANDLE fenceEvent);
		void begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList, D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView);
		Microsoft::WRL::ComPtr<ID3D12Resource2> getConstantBuffer();
		void end(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList);
		void signal(Microsoft::WRL::ComPtr<ID3D12CommandQueue1> commandQueue);
		
		~FrameBuffer();
	};
}
