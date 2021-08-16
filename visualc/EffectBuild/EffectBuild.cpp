// EffectBuild.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <d3dcompiler.h>

#include <iostream>
#include <fstream>
#include <map>

#include "IncludeHandler.h"

void print_usage()
{
	printf("USAGE:\n");
	printf("\tefb.exe <parameters> inputfile.fx <outputfile.fxo>\n");
	printf("PARAMETERS:\n");
	printf("\t/I \"<path>\" (add include path)\n");
	printf("\t/Od (disable optimization)\n");
	printf("\t/O0, /O1, /O2, /O3 (optimization levels. O1 is the default setting)\n");
	printf("\t/Vd (disable validation)\n");
	printf("\t/WX (treat warnings as errors)\n");
	printf("\t/Zpc (pack matrices in column-major order)\n");
	printf("\t/Zpr (pack matrices in row-major order)\n");
}

int main(int argc, char* argv[])
{
	printf("EffectBuild FX 2.0 HLSL effect compiler\n");

	if (argc <= 1)
	{
		print_usage();
		return 0;
	}

	int opt = 1;
	bool validate = true;
	bool warningAsError = false;
	int packMode = 0;

	std::string inFile = "";
	std::string outFile = "";

	IncludeHandler* handler = new IncludeHandler();

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "/Od") == 0)
		{
			opt = -1;
		}
		else if (strcmp(argv[i], "/O0") == 0)
		{
			opt = 0;
		}
		else if (strcmp(argv[i], "/O1") == 0)
		{
			opt = 1;
		}
		else if (strcmp(argv[i], "/O2") == 0)
		{
			opt = 2;
		}
		else if (strcmp(argv[i], "/O3") == 0)
		{
			opt = 3;
		}
		else if (strcmp(argv[i], "/Vd") == 0)
		{
			validate = false;
		}
		else if (strcmp(argv[i], "/WX") == 0)
		{
			warningAsError = true;
		}
		else if (strcmp(argv[i], "/Zpc") == 0)
		{
			packMode = 1;
		}
		else if (strcmp(argv[i], "/Zpr") == 0)
		{
			packMode = 2;
		}
		else if (strcmp(argv[i], "/I") == 0)
		{
			if (i + 1 >= argc)
			{
				printf("/I requires include path\n");
				print_usage();
				return 1;
			}

			i++;

			printf("Adding include path: %s\n", argv[i]);
			handler->AddPath(argv[i]);
		}
		else if (inFile.empty())
		{
			inFile = argv[i];
		}
		else if (outFile.empty())
		{
			outFile = argv[i];
		}
	}

	if (inFile.empty())
	{
		delete handler;

		printf("efb.exe requires input file path\n");
		print_usage();
		return 1;
	}

	// set up compiler flags
	UINT flags = 0;

	if (opt == -1)
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	else if (opt == 0)
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
	else if (opt == 1)
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
	else if (opt == 2)
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
	else if (opt == 3)
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	if (validate == false)
		flags |= D3DCOMPILE_SKIP_VALIDATION;

	if (warningAsError)
		flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

	if (packMode == 1)
		flags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
	else if(packMode == 2)
		flags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	// make outfile just be inFile with a different extension
	if (outFile.empty())
	{
		outFile = (inFile.find('.') == std::string::npos) ?
			inFile + ".fxo" :
			inFile.substr(0, inFile.find_last_of('.')) + ".fxo";
	}

	// open input shader file
	std::ifstream inStream;
	inStream.open(inFile, std::ios::in | std::ios::binary | std::ios::ate);
	if (!inStream.is_open())
	{
		printf("Failed to open input file %s\n", inFile.c_str());
		return 1;
	}

	// read effect file into memory
	auto size = inStream.tellg();
	auto fileMem = new char[size];
	inStream.seekg(0, std::ios::beg);
	inStream.read(fileMem, size);
	inStream.close();

	// compile it!
	ID3DBlob* ppCode;
	ID3DBlob* ppErrorMsgs;

	HRESULT result = D3DCompile(fileMem, size, inFile.c_str(), nullptr, handler, nullptr, "fx_2_0", flags, 0, &ppCode, &ppErrorMsgs);

	delete [] fileMem;
	delete handler;

	if (result == S_OK)
	{
		// were there any warnings?
		if (ppErrorMsgs != nullptr)
		{
			char *msg = (char *)ppErrorMsgs->GetBufferPointer();
			printf(msg);
		}

		char* memPtr = (char*)ppCode->GetBufferPointer();
		size_t size = ppCode->GetBufferSize();

		// now try and write to output file
		std::ofstream outStream;
		outStream.open(outFile, std::ios::out | std::ios::binary | std::ios::trunc);

		if (!outStream.is_open())
		{
			printf("Failed to open output file %s\n", outFile.c_str());
			return 1;
		}

		outStream.write(memPtr, size);
		outStream.close();

		printf("Compile successful! Wrote %s (%d bytes)\n", outFile.c_str(), (int)size);
		return 0;
	}
	else
	{
		printf("Failed to compile:\n");
		char* msg = (char*)ppErrorMsgs->GetBufferPointer();
		printf(msg);
		return 1;
	}
}