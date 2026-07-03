#pragma once

#include "pch.h"

#ifdef _DEBUG
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

namespace tracer {
	inline void VERIFY(bool result) {
		if (!result) {
			exit(EXIT_FAILURE);
		}
	}

	inline void VERIFY_NOT(bool result) {
		if (result) {
			exit(EXIT_FAILURE);
		}
	}

	inline void VERIFY_WIN(bool result) {
		if (!result) {
			if (DEBUG) {
				auto error = GetLastError();
				char message[UCHAR_MAX] = {};
				VERIFY(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message, UCHAR_MAX, nullptr));
				std::println("{}", message);
			}
			exit(EXIT_FAILURE);
		}
	}

	inline void VERIFY_COM(HRESULT result) {
		if (FAILED(result)) {
			if (DEBUG) {
				std::println("{}", _com_error(result).ErrorMessage());
			}
			exit(EXIT_FAILURE);
		}
	}

	inline void VERIFY_GLTF(cgltf_result result) {
		if (result != cgltf_result_success) {
			if (result == cgltf_result_data_too_short) {
				std::println("Data too short");
			}
			else if (result == cgltf_result_unknown_format) {
				std::println("Unknown format");
			}
			else if (result == cgltf_result_invalid_json) {
				std::println("Invalid JSON");
			}
			else if (result == cgltf_result_invalid_gltf) {
				std::println("Invalid gLTF");
			}
			else if (result == cgltf_result_invalid_options) {
				std::println("Invalid options");
			}
			else if (result == cgltf_result_file_not_found) {
				std::println("File not found");
			}
			else if (result == cgltf_result_io_error) {
				std::println("IO error");
			}
			else if (result == cgltf_result_out_of_memory) {
				std::println("Out of memory");
			}
			else if (result == cgltf_result_legacy_gltf) {
				std::println("Legacy gLTF");
			}
			exit(EXIT_FAILURE);
		}
	}

	inline void VERIFY_SHADER(Microsoft::WRL::ComPtr<IDxcResult> result) {
		HRESULT status = {};
		VERIFY_COM(result->GetStatus(&status));

		if (FAILED(status)) {
			if (DEBUG) {
				Microsoft::WRL::ComPtr<IDxcBlobEncoding> message = nullptr;
				VERIFY_COM(result->GetErrorBuffer(message.GetAddressOf()));
				std::println("{}", reinterpret_cast<const char*>(message->GetBufferPointer()));
			}
			exit(EXIT_FAILURE);
		}
	}
}
