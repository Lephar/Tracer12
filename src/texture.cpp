#include "pch.h"

#include "texture.h"

#include "content.h"

#include "debug.h"

namespace tracer::content {
	struct Texture::Implementation {
		Type type;
		DirectX::ScratchImage image;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		Microsoft::WRL::ComPtr<ID3D12Resource2> buffer;
		Microsoft::WRL::ComPtr<ID3D12Resource2> texture;
		D3D12_CPU_DESCRIPTOR_HANDLE view;
	};

	namespace {
		DirectX::ScratchImage construct(std::filesystem::path path, Texture::Type type) {
			DirectX::ScratchImage image;
			debug::verify::com(DirectX::LoadFromWICFile(path.wstring().c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image, nullptr));

			auto& mainImage = *image.GetImages();
			debug::print("Image loaded: %ux%u", mainImage.width, mainImage.height);
			/*
			DirectX::TEX_COMPRESS_FLAGS flags = DirectX::TEX_COMPRESS_PARALLEL;

			if (type == Texture::Type::BASE_COLOR) {
				flags |= DirectX::TEX_COMPRESS_SRGB;
			}
			else if (type == Texture::Type::NORMAL) {
				flags |= DirectX::TEX_COMPRESS_UNIFORM;
			}

			DirectX::CompressOptions options = {
				.flags = flags,
				.threshold = DirectX::TEX_THRESHOLD_DEFAULT,
				.alphaWeight = DirectX::TEX_ALPHA_WEIGHT_DEFAULT,
			};

			DirectX::ScratchImage compressedImage;
			debug::verify::com(DirectX::CompressEx(mainImage, DXGI_FORMAT_BC5_UNORM, options, compressedImage, nullptr));

			image = std::move(compressedImage);
			debug::print("Image compressed");
			*/
			return image;
		}
	}
	Texture::Texture(std::filesystem::path folder, const char* file, Type type) : implementation(std::make_unique<Implementation>()) {
		debug::incrementDepth();

		debug::print("File: %s", file);
		auto path = folder / file;

		implementation->type = type;
		implementation->image = construct(path, implementation->type);

		debug::decrementDepth();
	}

	Texture::Texture(std::filesystem::path folder, cgltf_image* data, Type type) : implementation(std::make_unique<Implementation>()) {
		debug::incrementDepth();

		std::string uri{ data->uri };
		std::replace(uri.begin(), uri.end(), '/', '\\');

		if (data->name) {
			debug::print("Name: %s", data->name);
		}
		else {
			debug::print("Path: %s", uri.c_str());
		}

		auto path = folder / uri;

		implementation->type = type;
		implementation->image = construct(path, implementation->type);

		debug::decrementDepth();
	}

	Texture::Texture(Texture&& texture) noexcept : implementation(std::move(texture.implementation)) {}

	Texture& Texture::operator=(Texture&& texture) noexcept {
		implementation = std::move(texture.implementation);
		return *this;
	}

	void Texture::createResources(Microsoft::WRL::ComPtr<ID3D12Device15> device, D3D12_CPU_DESCRIPTOR_HANDLE textureView) {
		auto& metadata = implementation->image.GetMetadata();

		debug::verify::positive(DirectX::IsSupportedTexture(device.Get(), metadata));
		debug::print("Texture support checked");

		debug::verify::com(DirectX::CreateTextureEx(device.Get(), metadata, D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_DEFAULT, reinterpret_cast<ID3D12Resource**>(implementation->texture.GetAddressOf())));
		debug::print("Texture created");

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
		debug::print("Shader resource view created");

		debug::verify::com(PrepareUpload(device.Get(), implementation->image.GetImages(), implementation->image.GetImageCount(), metadata, implementation->subresources));
		debug::print("Texture prepared for upload");

		const uint64_t uploadBufferSize = GetRequiredIntermediateSize(implementation->texture.Get(), 0, static_cast<uint32_t>(implementation->subresources.size()));
		debug::print("Required buffer size acquired: %lu", uploadBufferSize);

		const auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto uploadBufferDesc = CD3DX12_RESOURCE_DESC1::Buffer(uploadBufferSize);
		debug::verify::com(device->CreateCommittedResource2(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(implementation->buffer.GetAddressOf())));
		debug::print("Texture upload heap created");
	}

	void Texture::recordUpload(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> commandList) {
		UpdateSubresources(commandList.Get(), implementation->texture.Get(), implementation->buffer.Get(), 0, 0, static_cast<uint32_t>(implementation->subresources.size()), implementation->subresources.data());
		debug::print("Upload command recorded");

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(implementation->texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		commandList->ResourceBarrier(1, &barrier);
		debug::print("Resource barrier recorded");
	}

	void Texture::clearStaging() {
		implementation->buffer.Reset();
		implementation->subresources.clear();
		implementation->image.Release();

		debug::print("Staging resources cleaned up");
	}

	Texture::~Texture() = default;
}
