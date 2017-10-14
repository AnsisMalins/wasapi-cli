#pragma once

std::string WideCharToUTF8(const std::wstring& str);
std::string WideCharToUTF8NoExcept(const std::wstring& str) noexcept;
std::wstring UTF8ToWideChar(const std::string& str);
std::wstring UTF8ToWideCharNoExcept(const std::string& str) noexcept;
