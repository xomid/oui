#include "UIDOpenFile.h"
#include "oui_uix.h"
#include <windows.h> 
#include <shobjidl.h> 

int UIDOpenFile::show_window(OUI* caller) 
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	int res = 1;

	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			IShellItem* stDefDir, * stDir;
			COMDLG_FILTERSPEC* ComDlgFS = NULL;
			HRESULT result;

			result = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&stDefDir));
			if (SUCCEEDED(result)) {
				pFileOpen->SetDefaultFolder(stDefDir);
			}

			result = SHCreateItemFromParsingName(directory.c_str(), NULL, IID_PPV_ARGS(&stDir));
			if (SUCCEEDED(result)) {
				pFileOpen->SetFolder(stDir);
			}

			DWORD optFlags = 0;
			for (auto& opt : options)
				optFlags |= (DWORD)opt;
			pFileOpen->SetOptions(optFlags);

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
					pFileOpen->SetFileTypes((UINT)fileTypesSz, ComDlgFS);
				}
			}

			if (defaultExtention.length()) pFileOpen->SetDefaultExtension(defaultExtention.c_str());
			if (fileName.length()) pFileOpen->SetFileName(fileName.c_str());
			if (fileNameLabel.length()) pFileOpen->SetFileNameLabel(fileNameLabel.c_str());
			if (title.length()) pFileOpen->SetTitle(title.c_str());
			if (btnOKText.length()) pFileOpen->SetOkButtonLabel(btnOKText.c_str());
			pFileOpen->SetFileTypeIndex(fileTypeIndex);

			// Show the Open dialog box.
			hr = pFileOpen->Show(caller && caller->uix ? caller->uix->get_hwnd() : 0);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItemArray* pItems;
				hr = pFileOpen->GetResults(&pItems);
				if (SUCCEEDED(hr))
				{
					DWORD sz;
					hr = pItems->GetCount(&sz);

					if (SUCCEEDED(hr))
					{
						for (DWORD i = 0; i < sz; ++i) {
							IShellItem* pItem;
							PWSTR pszFilePath;

							hr = pItems->GetItemAt(i, &pItem);
							if (SUCCEEDED(hr)) {
								hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
								// Display the file name to the user.
								if (SUCCEEDED(hr))
								{
									fileNames.push_back(std::wstring(pszFilePath));
									CoTaskMemFree(pszFilePath);
								}
								pItem->Release();
							}
						}
					}
					pItems->Release();
				}

				res = 0;
			}

			if (ComDlgFS) delete[] ComDlgFS;
			if (stDir) stDir->Release();
			if (stDefDir) stDefDir->Release();
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return res;
}
