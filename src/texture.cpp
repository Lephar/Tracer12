#include "pch.h"

#include "texture.h"

#include "system.h"
#include "graphics.h"
#include "memory.h"

#include "verify.h"

namespace tracer::graphics::content {
	struct Texture::Implementation {
		DirectX::ScratchImage image;
		Microsoft::WRL::ComPtr<ID3D12Resource2> stagingBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> texture;
	};

	Texture::Texture(const char* folder, cgltf_image* data) : implementation(std::make_unique<Implementation>()) {
		if (data->name) {
			std::println("\t\t\tName: {}", data->name);
		}

		std::string uri{ data->uri };
		std::replace(uri.begin(), uri.end(), '/', '\\');

		auto path = system::getDataFolder() / "assets" / folder / uri;
		std::println("\t\t\tPath: {}", path.string());

		DirectX::ScratchImage image = {};
		VERIFY_COM(DirectX::LoadFromWICFile(path.wstring().c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image, nullptr));
		std::println("\t\t\tImage loaded: {}x{}", image.GetImages()->width, image.GetImages()->height);

		const bool compress = false;

		if (compress) {
			DirectX::CompressOptions options = {
				.flags = DirectX::TEX_COMPRESS_PARALLEL,
				.threshold = DirectX::TEX_THRESHOLD_DEFAULT,
				.alphaWeight = DirectX::TEX_ALPHA_WEIGHT_DEFAULT,
			};

			VERIFY_COM(DirectX::CompressEx(*image.GetImages(), DXGI_FORMAT_BC5_UNORM, options, implementation->image, nullptr));
			std::println("\t\t\tImage compressed");
		}
		else {
			implementation->image = std::move(image);
		}

		auto device = getDevice();
		VERIFY(DirectX::IsSupportedTexture(device.Get(), implementation->image.GetMetadata()));
		std::println("\t\t\tTexture support checked");

		VERIFY_COM(DirectX::CreateTextureEx(device.Get(), implementation->image.GetMetadata(), D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_DEFAULT, reinterpret_cast<ID3D12Resource**>(implementation->texture.GetAddressOf())));
		std::println("\t\t\tTexture created");

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		VERIFY_COM(PrepareUpload(getDevice().Get(), implementation->image.GetImages(), implementation->image.GetImageCount(), implementation->image.GetMetadata(), subresources));
		std::println("\t\t\tTexture prepared for upload");

		const uint64_t uploadBufferSize = GetRequiredIntermediateSize(implementation->texture.Get(), 0, static_cast<uint32_t>(subresources.size()));
		std::println("\t\t\tRequired buffer size acquired: {}", uploadBufferSize);

		auto uploadHeapProperties = memory::getUploadHeapProperties();
		auto uploadBufferDesc = CD3DX12_RESOURCE_DESC1::Buffer(uploadBufferSize);
		VERIFY_COM(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(implementation->stagingBuffer.GetAddressOf())));
		std::println("\t\t\tTexture upload heap created");

		auto commandList = getCommandList();
		UpdateSubresources(commandList.Get(), implementation->texture.Get(), implementation->stagingBuffer.Get(), 0, 0, static_cast<uint32_t>(subresources.size()), subresources.data());
		std::println("\t\t\tTexture uploaded command recorded");

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		commandList->ResourceBarrier(1, &barrier);
		std::println("\t\t\tTexture barrier recorded");
	}

	Texture::~Texture() = default;
}
