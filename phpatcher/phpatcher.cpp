#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <sstream>
#include <iomanip>

using namespace std;

enum processHackerOffsets64
{
	TitleOffset64 = 0x10AED8,
	ClassOffset64 = 0x129100,
	DescriptionOffset64 = 0x19EAE4,
	IconOffset64 = 0x17CB18,
	qword_140159D10 = 0x20A1D,
	AppendAdminOffset64 = 0x227A1,
	PhTreeNewOffset64 = 0x129A70,
	MenuHackerOffset64 = 0x18F6E0,
	MenuProcessesOffset64 = 0x129A18,
	MenuServicesOffset64 = 0x129A30,
	MenuNetworkOffset64 = 0x129A48
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

string read_bytes(fstream* filestream, int offset, int length)
{
	filestream->seekg(offset, ios::beg);
	ostringstream hex_ss;
	hex_ss << hex << setfill('0');

	for (int i = 0; i < length; ++i) {
		hex_ss << setw(2) << unsigned((unsigned char)(filestream->get()));
	}

	return hex_ss.str();
}

void patch_bytes(fstream* filestream, int offset, string bytes_str)
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

	filestream->write(result.c_str(), result.length());
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

string gen_nop_string(int length)
{
	string out = "";
	for (int i = 0; i < length; i++)
		out += "90"; /*0x90 = nop*/
	out.resize(length * 2);
	return out;
}

int main()
{
	srand(time(0));

	fstream _64("64.exe", ios::in | ios::out | ios::binary);

	wstring name = gen_random<wstring>(14);
	wstring name_class = gen_random<wstring>(13);
	wstring description = gen_random<wstring>(14);
	wstring icon = gen_random<wstring>(14);
	wstring phtreenew = gen_random<wstring>(9);
	wstring mhacker = gen_random<wstring>(6);
	wstring mprocesses = gen_random<wstring>(9);
	wstring mservices = gen_random<wstring>(8);
	wstring mnetwork = gen_random<wstring>(7);

	if (!_64.is_open())
	{
		cout << "[-] failed to open 64.exe, make sure to rename your processhacker(x64 version) to 64.exe" << endl;
		cin.get();
		return 1;
	}

	string qword_140159D10_val = read_bytes(&_64, qword_140159D10, 0x2);
	string AppendAdminOffset64_val = read_bytes(&_64, AppendAdminOffset64, 0x18);

	cout << "[*] 0x20A1D | " << qword_140159D10_val << endl;
	cout << "[*] 0x227A1 | " << AppendAdminOffset64_val << endl;

	if (qword_140159D10_val.find("4889") == string::npos)
	{
		cout << "[*] this file is already patched, no need to patch again. aborting..." << endl;
		cin.get();
		return 1;
	}

	cout << "[*] applying patches..." << endl;

	patch_string(&_64, TitleOffset64, name.c_str());
	patch_string(&_64, ClassOffset64, name_class.c_str());
	patch_string(&_64, DescriptionOffset64, description.c_str());
	patch_string(&_64, IconOffset64, icon.c_str());
	patch_string(&_64, PhTreeNewOffset64, phtreenew.c_str());
	patch_string(&_64, MenuHackerOffset64, mhacker.c_str());
	patch_string(&_64, MenuProcessesOffset64, mprocesses.c_str());
	patch_string(&_64, MenuServicesOffset64, mservices.c_str());
	patch_string(&_64, MenuNetworkOffset64, mnetwork.c_str());

	patch_bytes (&_64, qword_140159D10, gen_nop_string(0x2));
	patch_bytes (&_64, AppendAdminOffset64, gen_nop_string(0x18));
	/*
		lea rdx, aAdministrator ; " (Administrator)"
		lea rcx, [rbp+57h+var_70]
		lea r8, [rax+rax]
		call PhAppendStringBuilderEx

		nop allat stuff
	*/

	cout << "[+] patched " << hex << TitleOffset64 << dec << endl;
	cout << "[+] patched " << hex << ClassOffset64 << dec << endl;
	cout << "[+] patched " << hex << DescriptionOffset64 << dec << endl;
	cout << "[+] patched " << hex << IconOffset64 << dec << endl;
	cout << "[+] patched " << hex << PhTreeNewOffset64 << dec << endl;
	cout << "[+] patched " << hex << MenuHackerOffset64 << dec << endl;
	cout << "[+] patched " << hex << MenuProcessesOffset64 << dec << endl;
	cout << "[+] patched " << hex << MenuServicesOffset64 << dec << endl;
	cout << "[+] patched " << hex << MenuNetworkOffset64 << dec << endl;

	cout << "[+] patched " << hex << qword_140159D10 << dec << endl;
	cout << "[+] patched " << hex << AppendAdminOffset64 << dec << endl;

	add_junk(&_64);

	qword_140159D10_val = read_bytes(&_64, qword_140159D10, 0x2);
	AppendAdminOffset64_val = read_bytes(&_64, AppendAdminOffset64, 0x18);

	_64.flush();
	_64.close();

	cout << "[+] done!" << endl;

	cout << "[*] 0x20A1D | " << qword_140159D10_val << endl;
	cout << "[*] 0x227A1 | " << AppendAdminOffset64_val << endl;

	cin.get();
}
