#pragma once

#ifdef _DEBUG
constexpr bool debug = true;
#else
constexpr bool debug = false;
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// STL
#include <print>
#include <vector>
#include <memory>
#include <chrono>
#include <numbers>
#include <fstream>
#include <filesystem>

// VC++
#include <windows.h>
#include <comdef.h>
#include <wrl/client.h>
#include <wincodec.h>
#include <dxgi1_6.h>

// DirectX
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dx12.h>
#include <dxcapi.h>
#include <DirectXMath.h>
#include <DirectXMesh.h>
#include <DirectXTex.h>
#include <DescriptorHeap.h>
