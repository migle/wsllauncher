// launcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "error.h"
#include "DlWsl.h"

const WslApiLoader wsl;

const auto Name = L"MGU-SDK";
const auto Title = L"MGU SDK";

const ULONG UidSdkDeveloper = 15000;
const WSL_DISTRIBUTION_FLAGS WslFlags = WSL_DISTRIBUTION_FLAGS_DEFAULT;

int wmain(int argc, wchar_t* argv[])
{
	// Log execution.
	std::wofstream logfile(std::wstring(argv[0]) + L".log");
	logfile << argv[0];
	for (int i = 1; i < argc; ++i)
		logfile << ' ' << argv[i];
	logfile << std::endl;

	// Update the title bar of the console window.
	SetConsoleTitleW(Title);

	// Select the distro-related action to be performed on this run.
	enum { reg, config, interactive, launch, runhere, unreg } action = interactive;
	int argi = 1; // first unconsumed command line argument.
	for (argi = 1; argi < argc && argi < 2; ++argi) {
		std::wstring_view arg = argv[argi];
		if (arg == L"reg" || arg == L"register") {
			action = reg;
		}
		else if (arg == L"config" || arg == L"configure") {
			action = config;
		}
		else if (arg == L"launch" || arg == L"interactive") {
			action = interactive;
		}
		else if (arg == L"run") {
			action = runhere;
		}
		else if (arg == L"unreg" || arg == L"unregister") {
			action = unreg;
		}
		else {
			std::wcerr << L"Usage: " << argv[0] << L" [register|configure|run|unregister]" << std::endl;
			return 1;
		}
	}

	// Behaviour now changes according to the selected action.
	HRESULT r = S_OK;
	DWORD exitCode = 0;
	if (SUCCEEDED(r)) switch (action) {
	default:
	case reg:
		std::wcout << "Registering " << Name << "..." << std::flush;
		r = wsl.RegisterDistribution(Name, argv[argi]);
		std::wcout << " done." << std::endl;
		break;
	case config:
		std::wcout << "Configuring " << Name << "..." << std::flush;
		if (argi < argc)
			r = wsl.ConfigureDistribution(Name, std::stoul(argv[argi]), WslFlags);
		else
			r = wsl.ConfigureDistribution(Name, UidSdkDeveloper, WslFlags);
		std::wcout << " done." << std::endl;
		break;
	case launch:
	case interactive:
		if (argi < argc)
			r = wsl.LaunchInteractive(Name, argv[argi], false, &exitCode);
		else
			r = wsl.LaunchInteractive(Name, L"", false, &exitCode);
		break;
	case runhere:
		if (argi < argc)
			r = wsl.LaunchInteractive(Name, argv[argi], true, &exitCode);
		else
			r = wsl.LaunchInteractive(Name, L"", true, &exitCode);
		break;
	case unreg:
		std::wcout << "Unregistering " << Name << "..." << std::flush;
		if (wsl.IsDistributionRegistered(Name))
			r = wsl.UnregisterDistribution(Name);
		std::wcout << " done." << std::endl;
		break;
	}

	// Exit code depends on success and exit code of child process.
	if (SUCCEEDED(r)) {
		return exitCode;
	}
	else {
		return 1;
	}
}
