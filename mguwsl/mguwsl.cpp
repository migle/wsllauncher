// mguwsl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

const auto Name = L"MGU-SDK";
const auto Title = L"MGU SDK";

ULONG defaultUID = 15000;
WSL_DISTRIBUTION_FLAGS wslFlags = WSL_DISTRIBUTION_FLAGS_DEFAULT;

HMODULE wslApiDll;

typedef BOOL(STDAPICALLTYPE* WSL_IS_DISTRIBUTION_REGISTERED)(PCWSTR);
typedef HRESULT(STDAPICALLTYPE* WSL_REGISTER_DISTRIBUTION)(PCWSTR, PCWSTR);
typedef HRESULT(STDAPICALLTYPE* WSL_UNREGISTER_DISTRIBUTION)(PCWSTR);
typedef HRESULT(STDAPICALLTYPE* WSL_CONFIGURE_DISTRIBUTION)(PCWSTR, ULONG, WSL_DISTRIBUTION_FLAGS);
typedef HRESULT(STDAPICALLTYPE* WSL_GET_DISTRIBUTION_CONFIGURATION)(PCWSTR, ULONG*, ULONG*, WSL_DISTRIBUTION_FLAGS*, PSTR**, ULONG*);
typedef HRESULT(STDAPICALLTYPE* WSL_LAUNCH)(PCWSTR, PCWSTR, BOOL, HANDLE, HANDLE, HANDLE, HANDLE*);
typedef HRESULT(STDAPICALLTYPE* WSL_LAUNCH_INTERACTIVE)(PCWSTR, PCWSTR, BOOL, DWORD*);

WSL_IS_DISTRIBUTION_REGISTERED wslIsDistributionRegistered;
WSL_REGISTER_DISTRIBUTION wslRegisterDistribution;
WSL_UNREGISTER_DISTRIBUTION wslUnregisterDistribution;
WSL_CONFIGURE_DISTRIBUTION wslConfigureDistribution;
WSL_GET_DISTRIBUTION_CONFIGURATION wslGetDistributionConfiguration;
WSL_LAUNCH wslLaunch;
WSL_LAUNCH_INTERACTIVE wslLaunchInteractive;

struct error
{
	DWORD id;
};

std::wostream& operator << (std::wostream& os, error e)
{
	// Format error message into internal buffer.
	std::array<wchar_t, 4096> buffer;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, e.id, 0,
		buffer.data(), DWORD(buffer.size()), nullptr);

	os << buffer.data() << std::endl;
	return os;
}

auto install_distro(const wchar_t* archive)
{
	// Register the distribution.
	const auto rr = wslRegisterDistribution(Name, archive);
	if (FAILED(rr))
		return rr;

	return HRESULT(0);
}

auto config_distro()
{
	return wslConfigureDistribution(Name, defaultUID, wslFlags);
}

auto interactive_distro()
{
	DWORD exitCode = 0;
	return wslLaunchInteractive(Name, L"", true, &exitCode);
}

auto uninstall_distro()
{
	// Just unregister.
	return wslUnregisterDistribution(Name);
}

int wmain(int argc, wchar_t* argv[])
{
	// Update the title bar of the console window.
	SetConsoleTitleW(Title);

	// Load the WslApi DLL.
	wslApiDll = LoadLibraryEx(L"wslapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (wslApiDll == nullptr) {
		std::wcerr << L"Error loading wslapi.dll: " << error{ GetLastError() } << std::endl;
		return 1;
	}
	
	// Get the API addresses in the DLL.
	wslIsDistributionRegistered = (WSL_IS_DISTRIBUTION_REGISTERED)GetProcAddress(wslApiDll, "WslIsDistributionRegistered");
	wslRegisterDistribution = (WSL_REGISTER_DISTRIBUTION)GetProcAddress(wslApiDll, "WslRegisterDistribution");
	wslUnregisterDistribution = (WSL_UNREGISTER_DISTRIBUTION)GetProcAddress(wslApiDll, "WslUnregisterDistribution");
	wslConfigureDistribution = (WSL_CONFIGURE_DISTRIBUTION)GetProcAddress(wslApiDll, "WslConfigureDistribution");
	wslGetDistributionConfiguration = (WSL_GET_DISTRIBUTION_CONFIGURATION)GetProcAddress(wslApiDll, "WslGetDistributionConfiguration");
	wslLaunch = (WSL_LAUNCH)GetProcAddress(wslApiDll, "WslLaunch");
	wslLaunchInteractive = (WSL_LAUNCH_INTERACTIVE)GetProcAddress(wslApiDll, "WslLaunchInteractive");

	// Select the distro-related action to be performed on this run.
	enum { install, config, launch, interactive, uninstall } action = install;
	for (int i = 1; i < argc; ++i) {
		std::wstring_view arg = argv[i];
		if (arg == L"install") {
			action = install;
			break;
		}
		else if (arg == L"config") {
			action = config;
			break;
		}
		else if (arg == L"run" || arg == L"launch") {
			action = interactive;
			break;
		}
		else if (arg == L"uninstall") {
			action = uninstall;
			break;
		}
		else {
			std::wcerr << L"Usage: " << argv[0] << L" [install|config|run|uninstall]" << std::endl;
		}
	}

	// Behaviour now changes according to the selected action.
	HRESULT r = S_OK;
	switch (action) {
	default:
	case install:
		r = install_distro(L"install.tar.gz");
		break;
	case config:
		r = config_distro();
		break;
	case launch:
	case interactive:
		r = interactive_distro();
		break;
	case uninstall:
		r = uninstall_distro();
		break;
	}

	// Check for error.
	if (FAILED(r)) {
		// Error.
		std::wcerr << error{ DWORD(r) } << std::endl;
		return 1;
	}
	else {
		// This is a successful run.
		return 0;
	}
}
