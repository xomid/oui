#pragma once
#include <vector>
#include <string>

#ifdef OUI_EXPORTS
#define OUI_API __declspec(dllexport)
#else
#define OUI_API// __declspec(dllimport)
#endif

#define INT_IS_ZERO(v) ((v) == 0)
#define INT_NOT_ZERO(v) ((v) != 0)
#define BYTE_OPAQUE(v) ((v) == 0xff)
#define BYTE_NOT_OPAQUE(v) ((v) != 0xff)
#define IS_NULL(v) (v == NULL)
#define IS_NOT_NULL(v) (v != NULL)
#define __SL(x) ((int)(x) >> 6)
#define TOABSX(relX) ((relX) + area.left)
#define TOABSY(relY) ((relY) + area.top)
#define TORELX(absX, a) ((absX) - a.left)
#define TORELY(absY, a) ((absY) - a.top)
#define SHEET_OVERLAP(a, s) (((0 <= a.left && a.left <= (s).w) || (a.left <= 0 && 0 <= a.left + a.width)) && ((0 <= a.top && a.top <= (s).h) || (a.top <= 0 && 0 <= a.top + a.height)))
#define AREA_OVERLAP(a, b) (((b.left <= a.left && a.left <= b.w) || (a.left <= b.left && b.left <= a.w)) && ((b.top <= a.top && a.top <= b.height) || (a.top <= b.top && b.top <= a.height)))
#define SGN(x) ((x) < 0 ? -1 : 1)
#define PI  3.14159265358979323846
#define PI_D2  (PI / 2)
#define Max(a,b) (((a) > (b)) ? (a) : (b))
#define Min(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a) ((a) < 0 ? -(a) : (a))
#define CEIL(v, d) ((v) / (d) + ((v) % (d) > 0 ? 1 : 0))
#define REM(v, d) ((v) % (d))
#define ROUND(v, d) ((v) / (d) + (ABS(v) % ABS(d) > 0 ? ((v) >= 0 && (d) >= 0 ? 1 : -1) : 0))
#define FLOOR(v, d) ((v) / (d))
#define HYP(x, y) (x = ABS(x), y = ABS(y), x > y ? x + (3 * y >> 3) : y + (3 * x >> 3))
#define TRANSFORM(x, offset, scale) (((x) + (offset)) * (scale))
#define SWAP(a, b) {int t = a; a = b; b = t;}
#define TOUINT(r, g, b, a) (((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g & 0xff)) << 8) | ((b) & 0xff)
#define TOUINT2(a, b) ((((a) & 0xffff) << 16) | ((b) & 0xffff))
#define TOULONG2(a, b) (((((uint64_t)a) & 0xffffffff) << 32) | (((uint64_t)b) & 0xffffffff))
#define DIV255(v) (((v) + 1 + (((v) + 1) >> 8)) >> 8)
#define CLAMP255(v) (byte)((v) > 0xff ? 0xff : ((v) < 0 ? 0 : (v)))
#define CLAMP3(mn, n, mx) ((n) > (mx) ? (mx) : ((n) < (mn) ? (mn) : (n)))
#define CLAMP2(n, mx) ((long)(n) > (long)(mx) ? (long)(mx) : ((long)(n) < 0L ? 0L : (long)(n)))
#define iterateI(n) for (size_t i = 0, _n = (n); i < _n; ++i) 
#define iterateIRev(n) for (int i = int(n) - 1; i > -1; --i) 
#define iterateV(a) for(auto it = a.begin(); it != a.end(); ++it) 
#define iterateVRev(a) for(auto it = a.rbegin(); it != a.rend(); ++it) 

class OUI;
struct Spacing;
typedef uint8_t byte;
typedef byte* pyte;

namespace oui {
	static char buff[2048];

	static inline bool is_zeroi(int var) {
		return var == 0;
	}
	static inline bool is_zerof(float var) {
		return fabsf(var) < 0.0001;
	}
	static inline bool is_zerod(double var) {
		return fabs(var) < 0.00000001;
	}

	OUI_API char to_single_hex(byte number);
	OUI_API std::string to_hex(byte number);
	OUI_API std::string to_hex(byte r, byte g, byte b, byte a);
	OUI_API bool is_in_range(double num, double min, double max, bool min_inclusive = true, bool max_inclusive = false);
	OUI_API int find(const char* str, size_t lenstr, const char* q, size_t lenq);
	OUI_API uint32_t count_char_outside_paranthesis(std::string& text, char ch);
	OUI_API bool begin(const char* string, const char* prefix, bool caseSensitive);
	OUI_API std::string to_string(std::wstring wstr);
	OUI_API std::wstring to_wstring(std::string str);

}
