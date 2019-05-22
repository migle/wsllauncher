#pragma once

#include <windows.h>

/// Dynamic loader and wrapper for the WSL API.
///
/// The wslapi.dll might not be available on the system, and the import library for it appears not to be yet in the
/// Windows SDK.
/// For that reason, and following Microsoft's sample code, we use this wrapper class where we keep the entry points
/// of the API and is responsible for loading and freeing the DLL.
class WslApiLoader
{
	typedef BOOL(STDAPICALLTYPE* WSL_IS_DISTRIBUTION_REGISTERED)(PCWSTR);
	typedef HRESULT(STDAPICALLTYPE* WSL_REGISTER_DISTRIBUTION)(PCWSTR, PCWSTR);
	typedef HRESULT(STDAPICALLTYPE* WSL_UNREGISTER_DISTRIBUTION)(PCWSTR);
	typedef HRESULT(STDAPICALLTYPE* WSL_GET_DISTRIBUTION_CONFIGURATION)(PCWSTR, ULONG*, ULONG*, WSL_DISTRIBUTION_FLAGS*, PSTR**, ULONG*);
	typedef HRESULT(STDAPICALLTYPE* WSL_CONFIGURE_DISTRIBUTION)(PCWSTR, ULONG, WSL_DISTRIBUTION_FLAGS);
	typedef HRESULT(STDAPICALLTYPE* WSL_LAUNCH_INTERACTIVE)(PCWSTR, PCWSTR, BOOL, DWORD*);
	typedef HRESULT(STDAPICALLTYPE* WSL_LAUNCH)(PCWSTR, PCWSTR, BOOL, HANDLE, HANDLE, HANDLE, HANDLE*);

	HMODULE _hModule;

	WSL_IS_DISTRIBUTION_REGISTERED _isDistributionRegistered;
	WSL_REGISTER_DISTRIBUTION _registerDistribution;
	WSL_UNREGISTER_DISTRIBUTION _unregisterDistribution;
	WSL_GET_DISTRIBUTION_CONFIGURATION _getDistributionConfiguration;
	WSL_CONFIGURE_DISTRIBUTION _configureDistribution;
	WSL_LAUNCH_INTERACTIVE _launchInteractive;
	WSL_LAUNCH _launch;

public:
	/// Constructor: loads the DLL.
	WslApiLoader();
	/// Destructor: frees the DLL module.
	~WslApiLoader();

	BOOL IsDistributionRegistered(PCWSTR) const;
	HRESULT RegisterDistribution(PCWSTR, PCWSTR) const;
	HRESULT UnregisterDistribution(PCWSTR) const;
	HRESULT GetDistributionConfiguration(PCWSTR, ULONG*, ULONG*, WSL_DISTRIBUTION_FLAGS*, PSTR**, ULONG*) const;
	HRESULT ConfigureDistribution(PCWSTR, ULONG, WSL_DISTRIBUTION_FLAGS) const;
	HRESULT LaunchInteractive(PCWSTR, PCWSTR, BOOL, DWORD*) const;
	HRESULT Launch(PCWSTR, PCWSTR, BOOL, HANDLE, HANDLE, HANDLE, HANDLE*) const;
};

