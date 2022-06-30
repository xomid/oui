#pragma once
#include "UIDFile.h"

class OUI_API UIDOpenFile : public UIDFile
{
public:
	int show_window(OUI* caller) override;
};

