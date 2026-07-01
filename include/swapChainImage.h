#pragma once

namespace tracer::graphics::swapChain {
	class Image {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Image(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, D3D12_CPU_DESCRIPTOR_HANDLE constantBufferHostView, D3D12_GPU_DESCRIPTOR_HANDLE constantBufferDeviceView, uint8_t* constantBufferMemory);
		
		Image(const Image& image) = delete;
		Image& operator=(const Image& image) = delete;

		Image(Image&& image) noexcept;
		Image& operator=(Image&& image) noexcept;

		void wait();
		void begin();
		void bind(D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView);
		void end();
		void signal();

		~Image();
	};
}
