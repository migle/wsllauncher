#include "pch.h"
#include "error.h"
#include "DlWsl.h"

WslApiLoader::WslApiLoader()
{
	/// Loads the wslapi.dll and retrieves the entry points of all functions.
	if ((_hModule = LoadLibraryEx(L"wslapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) == nullptr)
		throw win32_error();
	if ((_isDistributionRegistered = reinterpret_cast<WSL_IS_DISTRIBUTION_REGISTERED>(GetProcAddress(_hModule, "WslIsDistributionRegistered"))) == nullptr)
		throw win32_error();
	if ((_registerDistribution = reinterpret_cast<WSL_REGISTER_DISTRIBUTION>(GetProcAddress(_hModule, "WslRegisterDistribution"))) == nullptr)
		throw win32_error();
	if ((_unregisterDistribution = reinterpret_cast<WSL_UNREGISTER_DISTRIBUTION>(GetProcAddress(_hModule, "WslUnregisterDistribution"))) == nullptr)
		throw win32_error();
	if ((_getDistributionConfiguration = reinterpret_cast<WSL_GET_DISTRIBUTION_CONFIGURATION>(GetProcAddress(_hModule, "WslGetDistributionConfiguration"))) == nullptr)
		throw win32_error();
	if ((_configureDistribution = reinterpret_cast<WSL_CONFIGURE_DISTRIBUTION>(GetProcAddress(_hModule, "WslConfigureDistribution"))) == nullptr)
		throw win32_error();
	if ((_launchInteractive = reinterpret_cast<WSL_LAUNCH_INTERACTIVE>(GetProcAddress(_hModule, "WslLaunchInteractive"))) == nullptr)
		throw win32_error();
	if ((_launch = reinterpret_cast<WSL_LAUNCH>(GetProcAddress(_hModule, "WslLaunch"))) == nullptr)
		throw win32_error();
}

WslApiLoader::~WslApiLoader()
{
	FreeLibrary(_hModule);
}

BOOL WslApiLoader::IsDistributionRegistered(PCWSTR name) const
{
	return _isDistributionRegistered(name);
}

HRESULT WslApiLoader::RegisterDistribution(PCWSTR name, PCWSTR tarName) const
{
	HRESULT hr = _registerDistribution(name, tarName);
	if (FAILED(hr)) {
		std::wcerr << "WslRegisterDistribution failed: " << error{ DWORD(hr) } << std::endl;
	}
	return hr;
}

HRESULT WslApiLoader::UnregisterDistribution(PCWSTR name) const
{
	HRESULT hr = _unregisterDistribution(name);
	if (FAILED(hr)) {
		std::wcerr << "WslUnregisterDistribution failed: " << error{ DWORD(hr) } << std::endl;
	}
	return hr;
}

HRESULT WslApiLoader::GetDistributionConfiguration(PCWSTR name, ULONG* version, ULONG* uid, WSL_DISTRIBUTION_FLAGS* flags, PSTR** envp, ULONG* envc) const
{
	HRESULT hr = _getDistributionConfiguration(name, version, uid, flags, envp, envc);
	if (FAILED(hr)) {
		std::wcerr << "WslGetDistributionConfiguration failed: " << error{ DWORD(hr) } << std::endl;
	}
	return hr;
}

HRESULT WslApiLoader::ConfigureDistribution(PCWSTR name, ULONG uid, WSL_DISTRIBUTION_FLAGS flags) const
{
	HRESULT hr = _configureDistribution(name, uid, flags);
	if (FAILED(hr)) {
		std::wcerr << "WslConfigureDistribution failed: " << error{ DWORD(hr) } << std::endl;
	}
	return hr;
}

HRESULT WslApiLoader::LaunchInteractive(PCWSTR name, PCWSTR command, BOOL useCurrentWorkingDirectory, DWORD* exitCode) const
{
	HRESULT hr = _launchInteractive(name, command, useCurrentWorkingDirectory, exitCode);
	if (FAILED(hr)) {
		std::wcerr << "WslLaunchInteractive failed: " << error{ DWORD(hr) } << std::endl;
	}
	return hr;
}

HRESULT WslApiLoader::Launch(PCWSTR name, PCWSTR command, BOOL useCurrentWorkingDirectory, HANDLE stdIn, HANDLE stdOut, HANDLE stdErr, HANDLE* process) const
{
	HRESULT hr = _launch(name, command, useCurrentWorkingDirectory, stdIn, stdOut, stdErr, process);
	if (FAILED(hr)) {
		std::wcerr << "WslLaunch failed: " << error{ DWORD(hr) } << std::endl;
	}
	return hr;
}
