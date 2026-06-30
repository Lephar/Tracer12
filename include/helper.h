#pragma once

#include "pch.h"

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
}
