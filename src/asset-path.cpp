// src/asset-path.cpp
#include "asset-path.hpp"
#include <filesystem>

#ifdef __EMSCRIPTEN__
std::string asset_path(const std::string& relative) {
    return "/" + relative; // matches --preload-file assets@/assets
}
#else
std::string asset_path(const std::string& relative) {
    // Resolve relative to the executable's own directory, not CWD
    static std::filesystem::path base = std::filesystem::current_path(); // see note below
    return (base / relative).string();
}
#endif