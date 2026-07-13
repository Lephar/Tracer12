#include "pch.h"

#include "primitive.h"

#include "content.h"
#include "graphics.h"

#include "verify.h"

namespace tracer::content {
	struct Primitive::Implementation {
		cgltf_size indexBegin;
		cgltf_size indexCount;
		cgltf_size vertexOffset;
	};

	Primitive::Primitive(cgltf_primitive* data) : implementation(std::make_unique<Implementation>()) {
		auto& indices = getIndices();
		auto& vertices = getVertices();

		implementation->indexBegin = indices.size();
		implementation->vertexOffset = vertices.size();

		auto& indexAccessor = data->indices;
		auto& indexView = indexAccessor->buffer_view;
		auto& indexBuffer = indexView->buffer;

		void* indexData = reinterpret_cast<uint8_t*>(indexBuffer->data) + indexView->offset + indexAccessor->offset;

		implementation->indexCount = indexAccessor->count;
		std::println("\t\t\t\t{} INDEX", indexAccessor->count);

		if (indexAccessor->component_type == cgltf_component_type_r_16 || indexAccessor->component_type == cgltf_component_type_r_16u) {
			auto indexDataShort = reinterpret_cast<uint16_t*>(indexData);

			for (cgltf_size indexIndex = 0; indexIndex < indexAccessor->count; indexIndex++) {
				indices.push_back(indexDataShort[indexIndex]);
			}
		}
		else if (indexAccessor->component_type == cgltf_component_type_r_32u) {
			auto indexDataInteger = reinterpret_cast<uint32_t*>(indexData);

			for (cgltf_size indexIndex = 0; indexIndex < indexAccessor->count; indexIndex++) {
				indices.push_back(indexDataInteger[indexIndex]);
			}
		}

		for (cgltf_size attributeIndex = 0; attributeIndex < data->attributes_count; attributeIndex++) {
			auto& attribute = data->attributes[attributeIndex];

			auto& attributeAccessor = attribute.data;
			auto& attributeView = attributeAccessor->buffer_view;
			auto& attributeBuffer = attributeView->buffer;

			void* attributeData = reinterpret_cast<uint8_t*>(attributeBuffer->data) + attributeView->offset + attributeAccessor->offset;

			auto numComponents = cgltf_num_components(attributeAccessor->type);

			if (vertices.size() == implementation->vertexOffset) {
				vertices.resize(implementation->vertexOffset + attributeAccessor->count);
			}

			VERIFY(vertices.size() == implementation->vertexOffset + attributeAccessor->count);
			std::println("\t\t\t\t{} {}[{}]", attributeAccessor->count, attribute.name, numComponents);

			if (attribute.type == cgltf_attribute_type_position) {
				VERIFY(numComponents == 3);
				auto positions = reinterpret_cast<DirectX::SimpleMath::Vector3*>(attributeData);

				for (cgltf_size positionIndex = 0; positionIndex < attributeAccessor->count; positionIndex++) {
					auto& position = positions[positionIndex];
					vertices.at(implementation->vertexOffset + positionIndex).position = DirectX::SimpleMath::Vector4{ position.x, position.y, position.z, 1.0f };
				}
			}
			else if (attribute.type == cgltf_attribute_type_tangent) {
				VERIFY(numComponents == 4);
				auto tangents = reinterpret_cast<DirectX::SimpleMath::Vector4*>(attributeData);

				for (cgltf_size tangentIndex = 0; tangentIndex < attributeAccessor->count; tangentIndex++) {
					vertices.at(implementation->vertexOffset + tangentIndex).tangent = tangents[tangentIndex];
				}
			}
			else if (attribute.type == cgltf_attribute_type_normal) {
				VERIFY(numComponents == 3);
				auto normals = reinterpret_cast<DirectX::SimpleMath::Vector3*>(attributeData);

				for (cgltf_size normalIndex = 0; normalIndex < attributeAccessor->count; normalIndex++) {
					auto& normal = normals[normalIndex];
					vertices.at(implementation->vertexOffset + normalIndex).normal = DirectX::SimpleMath::Vector4{ normal.x, normal.y, normal.z, 0.0f };
				}
			} /*
			else if (attribute.type == cgltf_attribute_type_color) {
				VERIFY(numComponents == 3 || numComponents == 4);

				if (numComponents == 3) {
					auto colors = reinterpret_cast<DirectX::SimpleMath::Vector3*>(attributeData);

					for (cgltf_size colorIndex = 0; colorIndex < attributeAccessor->count; colorIndex++) {
						auto& color = colors[colorIndex];
						vertices.at(implementation->vertexOffset + colorIndex).color = DirectX::SimpleMath::Vector4{ color.x, color.y, color.z, 1.0f };
					}
				}
				else if (numComponents == 4) {
					auto colors = reinterpret_cast<DirectX::SimpleMath::Vector4*>(attributeData);

					for (cgltf_size colorIndex = 0; colorIndex < attributeAccessor->count; colorIndex++) {
						auto& color = colors[colorIndex];
						vertices.at(implementation->vertexOffset + colorIndex).color = DirectX::SimpleMath::Vector4{ color.x, color.y, color.z, color.w };
					}
				}
			} */
			else if (attribute.type == cgltf_attribute_type_texcoord) {
				VERIFY(numComponents == 2);
				auto texcoords = reinterpret_cast<DirectX::SimpleMath::Vector2*>(attributeData);

				if (std::string(attribute.name).compare("TEXCOORD_0") == 0) {
					for (cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
						auto& texcoord = texcoords[texcoordIndex];

						vertices.at(implementation->vertexOffset + texcoordIndex).texcoord.x = texcoord.x;
						vertices.at(implementation->vertexOffset + texcoordIndex).texcoord.y = texcoord.y;
					}
				}
				else if (std::string(attribute.name).compare("TEXCOORD_1") == 0) {
					for (cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
						auto& texcoord = texcoords[texcoordIndex];

						vertices.at(implementation->vertexOffset + texcoordIndex).texcoord.z = texcoord.x;
						vertices.at(implementation->vertexOffset + texcoordIndex).texcoord.w = texcoord.y;
					}
				}
			}
		}
	}

	Primitive::Primitive(Primitive&& primitive) noexcept : implementation(std::move(primitive.implementation)) {}

	Primitive& Primitive::operator=(Primitive&& primitive) noexcept {
		implementation = std::move(primitive.implementation);
		return *this;
	}
	/*
	void Primitive::draw() {
		getCommandList()->DrawIndexedInstanced(implementation->indexCount, 1, implementation->indexBegin, implementation->vertexOffset, 0);
	}
	*/
	Primitive::~Primitive() = default;
}
