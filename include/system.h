#pragma once

#include "pch.h"

namespace tracer::system {
	void initialize(const char* title);

	uint32_t getWidth();
	uint32_t getHeight();

	HWND getWindow();
	
	std::filesystem::path getDataFolder();

	void prepareLoop();
	
	bool poll();
	
	float getTimeDelta();
	DirectX::SimpleMath::Vector2 getMouseMovement();
	DirectX::SimpleMath::Vector3 getKeyboardMovement();
}
