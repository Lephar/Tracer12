#pragma once

#include "pch.h"

namespace tracer::graphics::swapChain {
	const uint32_t getImageCount();

	const DXGI_FORMAT getDepthStencilFormat();
	const DXGI_FORMAT getRenderTargetFormat();

	void initialize();

	void resize();
	void destroy();
}
