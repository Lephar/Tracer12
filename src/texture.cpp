#include "pch.h"

#include "texture.h"

#include "content.h"

#include "verify.h"

namespace tracer::content {
	struct Texture::Implementation {
		DirectX::ScratchImage image;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		Microsoft::WRL::ComPtr<ID3D12Resource2> buffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> texture;
		D3D12_CPU_DESCRIPTOR_HANDLE view;
	};

	Texture::Texture(std::filesystem::path folder, cgltf_image* data, bool compress) : implementation(std::make_unique<Implementation>()) {
		if (data->name) {
			std::println("\t\t\tName: {}", data->name);
		}
		else {
			std::println("\t\t\tTexture:");
		}

		std::string uri{ data->uri };
		std::replace(uri.begin(), uri.end(), '/', '\\');
		std::println("\t\t\tURI: {}", uri);

		auto path = folder / uri;

		DirectX::ScratchImage image;
		VERIFY_COM(DirectX::LoadFromWICFile(path.wstring().c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image, nullptr));
		std::println("\t\t\tImage loaded: {}x{}", image.GetImages()->width, image.GetImages()->height);

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
	}

	Texture::Texture(Texture&& texture) noexcept : implementation(std::move(texture.implementation)) {}

	Texture& Texture::operator=(Texture&& texture) noexcept {
		implementation = std::move(texture.implementation);
		return *this;
	}

	void Texture::createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_HEAP_PROPERTIES uploadHeapProperties, D3D12_CPU_DESCRIPTOR_HANDLE textureView) {
		auto& metadata = implementation->image.GetMetadata();

		VERIFY(DirectX::IsSupportedTexture(device.Get(), metadata));
		std::println("\t\t\tTexture support checked");

		VERIFY_COM(DirectX::CreateTextureEx(device.Get(), metadata, D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_DEFAULT, reinterpret_cast<ID3D12Resource**>(implementation->texture.GetAddressOf())));
		std::println("\t\t\tTexture created");

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {
			.Format = metadata.format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MostDetailedMip = 0,
				.MipLevels = 1,
				.PlaneSlice = 0,
				.ResourceMinLODClamp = 0.0f,
			},
		};

		implementation->view = textureView;
		device->CreateShaderResourceView(implementation->texture.Get(), &shaderResourceDesc, implementation->view);
		std::println("\t\t\tShader resource view created");

		VERIFY_COM(PrepareUpload(device.Get(), implementation->image.GetImages(), implementation->image.GetImageCount(), implementation->image.GetMetadata(), implementation->subresources));
		std::println("\t\t\tTexture prepared for upload");

		const uint64_t uploadBufferSize = GetRequiredIntermediateSize(implementation->texture.Get(), 0, static_cast<uint32_t>(implementation->subresources.size()));
		std::println("\t\t\tRequired buffer size acquired: {}", uploadBufferSize);

		auto uploadBufferDesc = CD3DX12_RESOURCE_DESC1::Buffer(uploadBufferSize);
		VERIFY_COM(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(implementation->buffer.GetAddressOf())));
		std::println("\t\t\tTexture upload heap created");
	}

	void Texture::recordUpload(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		UpdateSubresources(commandList.Get(), implementation->texture.Get(), implementation->buffer.Get(), 0, 0, static_cast<uint32_t>(implementation->subresources.size()), implementation->subresources.data());
		std::println("\t\t\tTexture uploaded command recorded");

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		commandList->ResourceBarrier(1, &barrier);
		std::println("\t\t\tTexture barrier recorded");
	}

	Texture::~Texture() = default;
}
