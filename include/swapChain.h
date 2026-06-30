#pragma once

#include "pch.h"

namespace tracer::graphics::swapChain {
	void initialize();

	DXGI_FORMAT getDepthStencilFormat();
	DXGI_FORMAT getRenderTargetFormat();

	void destroy();
}
