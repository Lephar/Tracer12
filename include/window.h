#pragma once

#include "pch.h"

namespace window {
	void createWindow();

	uint32_t getWidth();
	uint32_t getHeight();
	
	HWND getWindow();
	HANDLE getEvent();

	bool poll();
}
