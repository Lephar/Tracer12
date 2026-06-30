#pragma once

#include "pch.h"

namespace tracer::system {
	void initialize();

	uint32_t getWidth();
	uint32_t getHeight();
	
	HWND getWindow();
	HANDLE getEvent();

	std::filesystem::path getDataFolder();

	bool poll();
}
