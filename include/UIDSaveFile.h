#pragma once
#include "UIDFile.h"

class OUI_API UIDSaveFile : public UIDFile
{
public:
	int show_window(OUI* caller) override;
};

