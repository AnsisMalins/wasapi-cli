#pragma once

class Win32Exception : public std::exception
{
public:
    Win32Exception(const std::string& what, DWORD errorCode);
    DWORD ErrorCode() const { return m_ErrorCode; }
    const char* what() const override { return m_What.c_str(); }

protected:
    DWORD m_ErrorCode;
    std::string m_What;
};

BOOL Check(BOOL success, const char* file, int line);
