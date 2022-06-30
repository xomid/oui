#include "UIDSaveFile.h"
#include "oui_uix.h"
#include <windows.h> 
#include <shobjidl.h> 

int UIDSaveFile::show_window(OUI* caller)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	int res = 1;

	if (SUCCEEDED(hr))
	{
		IFileSaveDialog* pFileSave;

		// Create the FileSaveDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{
			IShellItem* stDefDir, * stDir;
			COMDLG_FILTERSPEC* ComDlgFS = NULL;
			HRESULT result;

			result = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&stDefDir));
			if (SUCCEEDED(result)) {
				pFileSave->SetDefaultFolder(stDefDir);
			}

			result = SHCreateItemFromParsingName(directory.c_str(), NULL, IID_PPV_ARGS(&stDir));
			if (SUCCEEDED(result)) {
				pFileSave->SetFolder(stDir);
			}

			DWORD optFlags = 0;
			for (auto& opt : options)
				optFlags |= (DWORD)opt;
			pFileSave->SetOptions(optFlags);

			size_t fileTypesSz;
			if (fileTypesSz = fileTypes.size()) {
				ComDlgFS = new COMDLG_FILTERSPEC[fileTypesSz];
				if (ComDlgFS) {
					for (int i = 0; i < fileTypesSz; ++i) {
						auto& type = fileTypes[i];
						auto& comFileType = ComDlgFS[i];
						comFileType.pszName = type.title.c_str();
						comFileType.pszSpec = type.extention.c_str();
					}
					pFileSave->SetFileTypes((UINT)fileTypesSz, ComDlgFS);
				}
			}

			if (defaultExtention.length()) pFileSave->SetDefaultExtension(defaultExtention.c_str());
			if (fileName.length()) pFileSave->SetFileName(fileName.c_str());
			if (fileNameLabel.length()) pFileSave->SetFileNameLabel(fileNameLabel.c_str());
			if (title.length()) pFileSave->SetTitle(title.c_str());
			if (btnOKText.length()) pFileSave->SetOkButtonLabel(btnOKText.c_str());
			pFileSave->SetFileTypeIndex(fileTypeIndex);

			// Show the Save dialog box.
			hr = pFileSave->Show(caller && caller->uix ? caller->uix->get_hwnd() : 0);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				PWSTR pszFilePath;

				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						fileNames = { std::wstring(pszFilePath) };
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}

				res = 0;
			}

			if (ComDlgFS) delete[] ComDlgFS;
			if (stDir) stDir->Release();
			if (stDefDir) stDefDir->Release();
			pFileSave->Release();
		}
		CoUninitialize();
	}

	return res;
}
