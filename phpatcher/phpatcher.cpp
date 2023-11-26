#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <sstream>

using namespace std;

enum processHackerOffsets64
{
	TitleOffset64 = 0x10AED8,
	ClassOffset64 = 0x129100,
	DescriptionOffset64 = 0x19EAE4,
	IconOffset64 = 0x17CB18,
	qword_140159D10 = 0x20A1D
};

template<typename t>
t gen_random(int len) {
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"01235467890";
	t tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}

void patch_bytes(fstream* filestream, int offset, std::string bytes_str)
{
	filestream->seekg(offset, ios::beg);

	basic_string<unsigned char> bytes;

	for (size_t i = 0; i < bytes_str.length(); i += 2)
	{
		uint16_t byte;
		istringstream(bytes_str.substr(i, 2)) >> hex >> byte;
		bytes.push_back(static_cast<unsigned char>(byte));
	}

	string result(begin(bytes), end(bytes));

	filestream->write(result.c_str(), result.length() * 2);
}

void patch_string(fstream* filestream, int offset, wstring string)
{
	filestream->seekg(offset, ios::beg);
	filestream->write((const char*)string.c_str(), string.length() * 2);
}

void add_junk(fstream* filestream)
{
	filestream->seekg(0, ios::end);
	string rnd = gen_random<string>(rand() % 133713371337);
	filestream->write(rnd.c_str(), rnd.length());
}

int main()
{
	srand(time(0));

	fstream _32;
	fstream _64("64.exe", ios::in | ios::out | ios::binary);

	wstring name = gen_random<wstring>(14);
	wstring name_class = gen_random<wstring>(13);
	wstring description = gen_random<wstring>(14);
	wstring icon = gen_random<wstring>(14);

	if (!_64.is_open())
	{
		MessageBoxA(0, "failed to open 64.exe, make sure to rename your processhacker(x64 version) to 64.exe", 0, MB_ICONERROR);
		return 1;
	}

	patch_string(&_64, TitleOffset64, name.c_str());
	patch_string(&_64, ClassOffset64, name_class.c_str());
	patch_string(&_64, DescriptionOffset64, description.c_str());
	patch_string(&_64, IconOffset64, icon.c_str());

	patch_bytes (&_64, qword_140159D10, "9090" /*0x90 = nop*/);

	add_junk(&_64);
}
