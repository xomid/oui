#pragma once
#include <string>
#include <vector>
#include "oui.h"

OUI_API enum class FileDialogOptions {
	OVERWRITEPROMPT = 0x2,
	STRICTFILETYPES = 0x4,
	NOCHANGEDIR = 0x8,
	PICKFOLDERS = 0x20,
	FORCEFILESYSTEM = 0x40,
	ALLNONSTORAGEITEMS = 0x80,
	NOVALIDATE = 0x100,
	ALLOWMULTISELECT = 0x200,
	PATHMUSTEXIST = 0x800,
	FILEMUSTEXIST = 0x1000,
	CREATEPROMPT = 0x2000,
	SHAREAWARE = 0x4000,
	NOREADONLYRETURN = 0x8000,
	NOTESTFILECREATE = 0x10000,
	HIDEMRUPLACES = 0x20000,
	HIDEPINNEDPLACES = 0x40000,
	NODEREFERENCELINKS = 0x100000,
	OKBUTTONNEEDSINTERACTION = 0x200000,
	DONTADDTORECENT = 0x2000000,
	FORCESHOWHIDDEN = 0x10000000,
	DEFAULTNOMINIMODE = 0x20000000,
	FORCEPREVIEWPANEON = 0x40000000,
	SUPPORTSTREAMABLEITEMS = 0x80000000
};

OUI_API struct FileDialogFileType {
	std::wstring title, extention;
};

class OUI_API UIDFile
{
protected:
	std::wstring title, defaultDirectory, directory, btnOKText, fileName, fileNameLabel, defaultExtention;
	std::vector<FileDialogFileType> fileTypes;
	uint32_t fileTypeIndex;
	std::vector<std::wstring> fileNames;
	std::vector<FileDialogOptions> options;

public:
	void set_title(std::wstring title);
	void set_file_types(std::vector<FileDialogFileType> fileTypes);
	void set_type_index(uint32_t fileTypeIndex);
	void set_default_dir(std::wstring defaultDirectory);
	void set_directory(std::wstring directory);
	void set_ok_button_text(std::wstring btnOKText);
	void set_file_name(std::wstring fileName);
	void set_file_name_label(std::wstring fileNameLabel);
	void set_default_extention(std::wstring defaultExtention);
	void set_options(std::vector<FileDialogOptions> options);

	std::vector<std::wstring> get_selected_file_names();

	virtual int show_window(OUI* caller);
};

