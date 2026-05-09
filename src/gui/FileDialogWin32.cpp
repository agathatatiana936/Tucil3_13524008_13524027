#include "gui/FileDialog.hpp"
#include "gui/portable-file-dialogs.h"

std::string pickFileOpen() {
    auto f = pfd::open_file("Select Input File", ".",
                            {"Text Files", "*.txt"},
                            pfd::opt::none);
    std::vector<std::string> res = f.result();
    if (!res.empty()) return res[0];
    return "";
}

std::string pickFileSave() {
    auto f = pfd::save_file("Save Solution", "solution.txt",
                            {"Text Files", "*.txt"},
                            pfd::opt::force_overwrite);
    std::string path = f.result();
    if (!path.empty() && path.size() < 4) {
        path += ".txt";
    }
    return path;
}
