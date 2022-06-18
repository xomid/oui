#pragma once
#include <vector>
#include <string>
#include <ocom.h>

#ifdef oui_EXPORTS
#define OUI_API __declspec(dllexport)
#else
#define OUI_API// __declspec(dllimport)
#endif
#pragma warning(disable:4251)

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

class OUI;
struct Spacing;