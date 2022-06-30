#include "UIDFile.h"

std::vector<std::wstring> UIDFile::get_selected_file_names() {
    return fileNames;
}

void UIDFile::set_title(std::wstring title) {
    this->title = title;
}

void UIDFile::set_file_types(std::vector<FileDialogFileType> fileTypes) {
    this->fileTypes = fileTypes;
}

void UIDFile::set_type_index(uint32_t fileTypeIndex) {
    this->fileTypeIndex = fileTypeIndex;
}

void UIDFile::set_default_dir(std::wstring defaultDirectory) {
    this->defaultDirectory = defaultDirectory;
}

void UIDFile::set_directory(std::wstring directory) {
    this->directory = directory;
}

void UIDFile::set_ok_button_text(std::wstring btnOKText) {
    this->btnOKText = btnOKText;
}

void UIDFile::set_file_name(std::wstring fileName) {
    this->fileName = fileName;
}

void UIDFile::set_file_name_label(std::wstring fileNameLabel) {
    this->fileNameLabel = fileNameLabel;
}

void UIDFile::set_default_extention(std::wstring defaultExtention) {
    this->defaultExtention = defaultExtention;
}

void UIDFile::set_options(std::vector<FileDialogOptions> options) {
    this->options = options;
}

int UIDFile::show_window(OUI* caller) {
    return 1;
}
