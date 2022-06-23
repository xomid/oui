#pragma once

#ifdef oui_EXPORTS
#define OUI_API __declspec(dllexport)
#elif defined(OUI_TEST_APP)
#define OUI_API //__declspec(dllimport)
#else
#define OUI_API __declspec(dllimport)
#endif
#pragma warning(disable:4251)
