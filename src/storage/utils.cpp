#include <filesystem>
#include "utils.h"

std::string storage_utils::file_name_from_table_name(const std::string &table_name) {
    // TODO - this needs to be in a user definable path, not PWD
    return table_name + k_table_file_extension;
}

std::vector<std::string> storage_utils::scan_table_names_from_files(const std::string &data_path_root) {
    std::vector<std::filesystem::directory_entry> table_files{};

    std::copy_if(
        std::filesystem::directory_iterator{data_path_root},
        std::filesystem::directory_iterator{},
        std::back_inserter(table_files),
        [](const auto &entry) {
            return entry.path().string().ends_with(k_table_file_extension);
        }
    );

    std::vector<std::string> table_names(table_files.size());
    std::transform(table_files.begin(), table_files.end(), table_names.begin(), [](const auto &file) {
        return file.path().stem().string();
    });

    return table_names;
}
