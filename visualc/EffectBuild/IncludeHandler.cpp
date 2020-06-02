#include <fstream>

#include "IncludeHandler.h"

HRESULT _stdcall IncludeHandler::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
	// try and open the input file. if that fails, try and open the input file relative to each include path.
	// if all of that fails, return error

	std::string filename = pFileName;

	std::ifstream inStream;
	inStream.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

	if (!inStream.is_open())
	{
		// make sure filename doesn't start with directory separator, that way we can just append to our include path strings (which already end with the separator)
		if (filename[0] == '/')
		{
			filename = filename.substr(1);
		}

		for (auto it = _includePath.begin(); it != _includePath.end(); it++)
		{
			inStream.open(*it + filename, std::ios::in | std::ios::binary | std::ios::ate);
			if (inStream.is_open())
				break;
		}
	}

	// couldn't find file, return failure
	if (!inStream.is_open())
	{
		return E_FAIL;
	}

	// file is open, read it in!
	auto size = inStream.tellg();
	auto fileMem = new char[size];
	inStream.seekg(0, std::ios::beg);
	inStream.read(fileMem, size);
	inStream.close();

	*ppData = fileMem;
	*pBytes = size;
	return S_OK;
}

HRESULT _stdcall IncludeHandler::Close(LPCVOID pData)
{
	char* buf = (char*)pData;
	delete[] buf;
	return S_OK;
}

void IncludeHandler::AddPath(std::string path)
{
	// make sure path ends with directory separator
	if (path[path.length() - 1] != '/')
		_includePath.push_back(path + "/");
	else
		_includePath.push_back(path);
}