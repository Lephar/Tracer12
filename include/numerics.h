#pragma once

#include "pch.h"

namespace tracer {
	uint64_t align(uint64_t value, uint64_t alignment);
	uint64_t alignBack(uint64_t value, uint64_t alignment);

	float radians(float degrees);
}
