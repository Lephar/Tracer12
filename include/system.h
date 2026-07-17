#pragma once

#include "pch.h"

namespace tracer::system {
	void initialize(const char* title);

	uint32_t getWidth();
	uint32_t getHeight();

	HWND getWindow();
	
	std::filesystem::path getDataFolder();
	
	bool poll();
	/*
	float getTimeDelta();
	DirectX::Mouse::State getMouseState();
	DirectX::Keyboard::State getKeyboardState();
	*/
}
