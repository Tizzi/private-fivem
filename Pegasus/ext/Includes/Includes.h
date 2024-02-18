#pragma once
#include <Windows.h>
#include <iostream>
#include <process.h>
#include <thread>
#include <vector>
#include <filesystem>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
//#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "user32.lib")
#include <mutex>
#include <map>
#include <unordered_map>


#include <imgui.h>
#include <imgui_internal.h>
#include <backend/imgui_impl_dx11.h>
#include <backend/imgui_impl_win32.h>
#include "imgui_notify.h"
#include "theme.h"
//#pragma comment(lib, "dwmapi.lib")

#include <Encryption.h>
#include <EncryptFuncs.hpp>
#include <xor.h>
#include <d3d11.h>
#include <Vectors.h>
#include <Memory.h>
#include <vehhk.h>

#include <hashes.hpp>
#include <Variables.h>
#include <Offsets.h>
#include <Classes.h>
#include <Pattern.h>
#include <joaat.hpp>
//#include <Obs_Streamproof.h>
#include <Config.h>
#include <Log.h>


inline ImVec2 MetricsSize = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

typedef HRESULT(__stdcall* OriginalFnPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

inline OriginalFnPresent oPresent{ 0 };
inline HWND hWnd{ 0 };
inline ID3D11Device* pDevice{ 0 };
inline ID3D11DeviceContext* pContext{ 0 };
inline ID3D11RenderTargetView* rendertarget{ 0 };
inline WNDPROC wnd_proc{ 0 };
inline DWORD_PTR swapchain_backup = 0x0;
inline uintptr_t* swapchain_vtables{ 0 };
inline DWORD dwOld = 0x0;