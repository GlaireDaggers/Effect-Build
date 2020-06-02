#pragma once

#include <d3dcompiler.h>

#include <string>
#include <vector>

class IncludeHandler : public ID3DInclude
{
private:
	std::vector<std::string> _includePath;

public:
	HRESULT _stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
	HRESULT _stdcall Close(LPCVOID pData);

public:
	void AddPath(std::string path);
};

