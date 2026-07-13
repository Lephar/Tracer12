#include "pch.h"

#include "debug.h"

namespace tracer::debug {
	namespace {
#ifdef _DEBUG
		const bool debug = true;
#else
		const bool debug = false;
#endif
		uint32_t depth = 0;
	}

	void resetDepth() {
		depth = 0;
	}

	void incrementDepth() {
		depth++;
	}

	void decrementDepth() {
		depth--;
	}

	void print(const char* fmt, ...) {
		if (debug) {
			for (uint32_t index = 0; index < depth; index++) {
				printf("\t");
			}

			va_list args;
			va_start(args, fmt);
			vprintf(fmt, args);
			va_end(args);

			printf("\n");
		}
	}

	namespace verify {
		void positive(bool result) {
			if (!result) {
				print("Failure");
				exit(EXIT_FAILURE);
			}
		}

		void negative(bool result) {
			positive(!result);
		}

		void win(bool result) {
			if (!result) {
				auto error = GetLastError();
				char message[UCHAR_MAX] = {};
				positive(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message, UCHAR_MAX, nullptr));
				print("%s", message);
				exit(EXIT_FAILURE);
			}
		}

		void com(HRESULT result) {
			if (FAILED(result)) {
				print("%s", _com_error(result).ErrorMessage());
				exit(EXIT_FAILURE);
			}
		}

		void dxc(Microsoft::WRL::ComPtr<IDxcResult> result) {
			HRESULT status = {};
			com(result->GetStatus(&status));

			if (FAILED(status)) {
				Microsoft::WRL::ComPtr<IDxcBlobEncoding> message = nullptr;
				com(result->GetErrorBuffer(message.GetAddressOf()));
				print("%s", message->GetBufferPointer());
				exit(EXIT_FAILURE);
			}
		}

		void gltf(cgltf_result result) {
			if (result != cgltf_result_success) {
				if (result == cgltf_result_data_too_short) {
					print("Data too short");
				}
				else if (result == cgltf_result_unknown_format) {
					print("Unknown format");
				}
				else if (result == cgltf_result_invalid_json) {
					print("Invalid JSON");
				}
				else if (result == cgltf_result_invalid_gltf) {
					print("Invalid glTF");
				}
				else if (result == cgltf_result_invalid_options) {
					print("Invalid options");
				}
				else if (result == cgltf_result_file_not_found) {
					print("File not found");
				}
				else if (result == cgltf_result_io_error) {
					print("IO error");
				}
				else if (result == cgltf_result_out_of_memory) {
					print("Out of memory");
				}
				else if (result == cgltf_result_legacy_gltf) {
					print("Legacy glTF");
				}
				exit(EXIT_FAILURE);
			}
		}
	}
}
