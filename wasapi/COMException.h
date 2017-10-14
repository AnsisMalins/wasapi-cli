#pragma once

class COMException final : public std::exception
{
public:
    COMException(const std::string& what, HRESULT errorCode);
    HRESULT ErrorCode() const { return m_ErrorCode; }
    const char* what() const override { return m_What.c_str(); }

private:
    std::string m_What;
	HRESULT m_ErrorCode;
};

HRESULT Check(HRESULT hr, const char* file, int line);
