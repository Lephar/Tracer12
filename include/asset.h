#pragma once

#include "pch.h"

namespace tracer::content {
	class Asset {
	private:
		struct Implementation;
		std::unique_ptr<Implementation> implementation;
	public:
		Asset(const char* folder, const char* file);

		Asset(const Asset& asset) = delete;
		Asset& operator=(const Asset& asset) = delete;

		Asset(Asset&& asset) noexcept;
		Asset& operator=(Asset&& asset) noexcept;

		//void draw();

		~Asset();
	};
}
