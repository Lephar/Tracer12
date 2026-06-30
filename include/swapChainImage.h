#pragma once

namespace tracer::graphics::swapChain {
	class Image {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Image(Microsoft::WRL::ComPtr<ID3D12Resource2> swapChainBuffer, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, Microsoft::WRL::ComPtr<ID3D12Fence1> fence);
		
		Image(const Image& image) = delete;
		Image& operator=(const Image& image) = delete;

		Image(Image&& image) noexcept;
		Image& operator=(Image&& image) noexcept;

		~Image();
	};
}
