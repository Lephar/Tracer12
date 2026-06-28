#pragma once

#include "pch.h"

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

inline void VERIFY_COM(HRESULT result) {
	if (FAILED(result)) {
		std::println("{}", _com_error(result).ErrorMessage());
		exit(EXIT_FAILURE);
	}
}
