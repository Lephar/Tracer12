#pragma once

namespace tracer {
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

		void setResources(Microsoft::WRL::ComPtr<ID3D12Resource2> renderTargetBuffer, Microsoft::WRL::ComPtr<ID3D12Resource2> resolveBuffer, Microsoft::WRL::ComPtr<ID3D12Resource2> constantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView);
		
		void wait();
		void begin(D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView);
		Microsoft::WRL::ComPtr<ID3D12Resource2> getConstantBuffer();
		void end();
		void signal();
		
		~FrameBuffer();
	};
}
