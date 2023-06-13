#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

enum processHackerOffsets32
{
	TitleOffset32 = 0xE0230,
	ClassOffset32 = 0xFD3AC,
	DescriptionOffset32 = 0x1552E4,
	IconOffset32 = 0x133318
};

enum processHackerOffsets64
{
	TitleOffset64 = 0x10AED8,
	ClassOffset64 = 0x129100,
	DescriptionOffset64 = 0x19EAE4,
	IconOffset64 = 0x17CB18
};

void patchBytes(fstream* filestream, int offset, const wchar_t* bytes)
{
	filestream->seekg(offset, ios::beg);
	filestream->write((const char*)bytes, wcslen(bytes) * 2);
}

std::wstring gen_random(int len) {
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::wstring tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}

int main()
{
	srand(time(0));

	fstream _32;
	fstream _64;

	_32.open("32.exe", ios::in | ios::out | ios::binary);
	_64.open("64.exe", ios::in | ios::out | ios::binary);

	std::wstring name = gen_random(14);
	std::wstring nameClass = gen_random(13);

	if (_32.is_open())
	{
		patchBytes(&_32, TitleOffset32, name.c_str());
		patchBytes(&_32, ClassOffset32, nameClass.c_str());
		patchBytes(&_32, DescriptionOffset32, name.c_str());
		patchBytes(&_32, IconOffset32, name.c_str());
	}

	if (_64.is_open())
	{
		patchBytes(&_64, TitleOffset64, name.c_str());
		patchBytes(&_64, ClassOffset64, nameClass.c_str());
		patchBytes(&_64, DescriptionOffset64, name.c_str());
		patchBytes(&_64, IconOffset64, name.c_str());
	}

	if (!_64.is_open() && !_32.is_open())
		MessageBoxA(0, "failed to find both '32.exe' and '64.exe'\r\nmake sure to rename Process Hacker version depending on its architecture to 32.exe or 64.exe", 0, MB_ICONWARNING | MB_OK);
}