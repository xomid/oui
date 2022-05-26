#include "oui_basics.h"
#include <codecvt>

namespace oui {

	OUI_API char to_single_hex(byte number) {
		if (number < 10) return '0' + number;
		else if (number < 16) return 'a' + (number - 10);
		return 'N';
	}

	OUI_API std::string to_hex(byte number) {
		std::string res = "";
		res += to_single_hex(number >> 4);
		return res + to_single_hex(number & 0xf);
	}

	OUI_API std::string to_hex(byte r, byte g, byte b, byte a) {
		return '#' + to_hex(r) + to_hex(g) + to_hex(b) + (a != 0xff ? to_hex(a) : "");
	}

	OUI_API bool is_in_range(double num, double min, double max, bool min_inclusive, bool max_inclusive) {
		return (min_inclusive ? num >= min : num > min) && (max_inclusive ? num <= max : num < max);
	}

	OUI_API bool begin(const char* string, const char* prefix, bool caseSensitive) {
		if (caseSensitive) return strncmp(string, prefix, strlen(prefix)) == 0;
		return _strnicmp(string, prefix, strlen(prefix)) == 0;
	}

	OUI_API int find(const char* str, size_t lenstr, const char* q, size_t lenq) {
		if (!str || !q || !lenq || !lenstr || lenstr < lenq) return -1;
		char first = q[0];
		for (int i = 0; i < lenstr - lenq + 1; ++i) {
			if (str[i] != first) continue;
			for (int j = 1; j < lenq; ++j) {
				if (str[i + j] != q[j])
					return -1;
			}
			return i;
		}
		return -1;
	}

	OUI_API uint32_t count_char_outside_paranthesis(std::string& text, char ch) {
		uint32_t count = 0;
		int open = 0;
		iterateI(text.length()) {
			if (text[i] == '(') ++open;
			else if (text[i] == ')') --open;
			else if (text[i] == ch && !open) ++count;
		}
		return count;
	}

	OUI_API std::string to_string(std::wstring wstr)
	{
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> strconverter;
		return strconverter.to_bytes(wstr);
	}

	OUI_API std::wstring to_wstring(std::string str)
	{
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> strconverter;
		return strconverter.from_bytes(str);
	}
}