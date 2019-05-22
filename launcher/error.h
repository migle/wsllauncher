#pragma once

#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <windows.h>

struct error
{
	DWORD id;
};

template < class CharT > inline
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& os, error e)
{
	std::array<CharT, 4096> buffer;
	format_message(buffer.data(), buffer.size(), e);
	os << buffer.data() << std::endl;
	return os;
}

inline std::size_t format_message(char* buf, std::size_t size, error e)
{
	return FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, e.id, 0, buf, DWORD(size), nullptr);
}

inline std::size_t format_message(wchar_t* buf, std::size_t size, error e)
{
	return FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, e.id, 0, buf, DWORD(size), nullptr);
}

inline std::string message(error e)
{
	std::stringstream ss;
	ss << e;
	return ss.str();
}

struct win32_error : std::runtime_error {
	win32_error(error e) : std::runtime_error(message(e)) {}
	win32_error(DWORD e) : win32_error(error{ e }) {}
	win32_error() : win32_error(GetLastError()) {}
};