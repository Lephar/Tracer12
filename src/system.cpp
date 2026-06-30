#include "pch.h"

#include "system.h"

#include "verify.h"

namespace tracer::system {
	namespace {
		const char* title = "Tracer";

		uint32_t width = 800;
		uint32_t height = 600;

		HINSTANCE instance = nullptr;
		HWND window = nullptr;
		HANDLE event = nullptr;

		std::filesystem::path dataFolder = {};

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			if (uMsg == WM_DESTROY) {
				PostQuitMessage(EXIT_SUCCESS);
				return EXIT_SUCCESS;
			}
			else if (WM_KEYDOWN) {
				if (wParam == VK_ESCAPE) {
					DestroyWindow(hWnd);
				}
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	void initialize() {
		VERIFY_WIN(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, nullptr, &instance));
		std::println("Instance handle acquired");

		WNDCLASSEX windowClass = {
			.cbSize = sizeof(WNDCLASSEX),
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = WndProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = instance,
			.hIcon = LoadIcon(nullptr, IDI_APPLICATION),
			.hCursor = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW),
			.lpszMenuName = nullptr,
			.lpszClassName = title,
			.hIconSm = LoadIcon(nullptr, IDI_APPLICATION)
		};

		VERIFY_WIN(RegisterClassEx(&windowClass));
		std::println("Window class registered");

		window = CreateWindowEx(0, title, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);
		VERIFY_WIN(window);
		std::println("Window created");

		VERIFY_NOT(ShowWindow(window, SW_SHOWDEFAULT));
		std::println("Window shown");

		VERIFY(UpdateWindow(window));
		std::println("Window updated");

		RECT rect;
		VERIFY_WIN(GetClientRect(window, &rect));
		width = rect.right;
		height = rect.bottom;
		std::println("Window size: {}x{}", width, height);

		event = CreateEvent(nullptr, false, false, nullptr);
		VERIFY_WIN(event);
		std::println("Event created");

		dataFolder = std::filesystem::current_path() / "data";
		std::println("Data folder set");
	}

	uint32_t getWidth() {
		return width;
	}

	uint32_t getHeight() {
		return height;
	}

	HWND getWindow() {
		return window;
	}

	HANDLE getEvent() {
		return event;
	}

	std::filesystem::path getDataFolder() {
		return dataFolder;
	}

	bool poll() {
		MSG message;

		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT) {
				return false;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return true;
	}
}
