#include "pch.h"

#include "numerics.h"

namespace tracer {
	uint64_t align(uint64_t value, uint64_t alignment) {
		return (value + alignment - 1) / alignment * alignment;
	}

	uint64_t alignBack(uint64_t value, uint64_t alignment) {
		return value / alignment * alignment;
	}

	float radians(float degrees) {
		return std::numbers::pi_v<float> * degrees / 180.0f;
	}
}
