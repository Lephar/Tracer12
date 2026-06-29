#pragma once

#include "pch.h"

namespace swapChain {
	void initialize();

	DXGI_FORMAT getDepthStencilFormat();
	DXGI_FORMAT getRenderTargetFormat();

	void destroy();
}
