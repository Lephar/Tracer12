#pragma once

#include "stdafx.h"

inline void VERIFY(bool result) {
	if (!result) {
		exit(EXIT_FAILURE);
	}
}

inline void VERIFY_COM(HRESULT result) {
	if (FAILED(result)) {
		std::println("{}", _com_error(result).ErrorMessage());
		exit(EXIT_FAILURE);
	}
}
