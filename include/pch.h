#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// CRT
#include <cstdio>
#include <cstdarg>

// STL
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <numbers>
#include <optional>
#include <filesystem>

// VC++
#include <windows.h>
#include <comdef.h>
#include <wrl/client.h>
#include <wincodec.h>

// DirectX
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dx12/d3dx12.h>
#include <directxtk12/DescriptorHeap.h>
#include <directxtk12/CommonStates.h>
#include <directxtk12/SimpleMath.h>
#include <directxtk12/Mouse.h>
#include <directxtk12/Keyboard.h>
#include <DirectXTex.h>

// External
#include <cgltf.h>
