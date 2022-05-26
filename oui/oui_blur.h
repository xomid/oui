#pragma once
#include "oui_basics.h"

OUI_API void stackblurJobS(byte* src, size_t w, size_t h, size_t radius, size_t step, byte* stack);

OUI_API void stackblurJob(byte* src, size_t w, size_t h, size_t radius, size_t step, byte* stack);

OUI_API void stackblurJobA(byte* src, size_t w, size_t h, size_t radius, size_t step, byte* stack);

OUI_API void stackblurJobG(byte* src, size_t w, size_t h, int pitch, size_t radius, size_t step, byte* stack);