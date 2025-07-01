#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    if(argc < 3) {
        std::cerr << "Usage: pak_bundler.exe <input_folder> <output_file>" << std::endl;
        return -1;
    }

    fs::path inputDir = argv[1];
    std::string outputFile = argv[2];
    std::ofstream out(outputFile, std::ios::binary);
    if(!out) throw std::runtime_error("Failed to create new pak file");

    // Entering the pak building part
    uint32_t version = 1;
    uint32_t numFiles = 0;

    for (auto& entry : fs::recursive_directory_iterator(inputDir)) {
        if (entry.is_regular_file()) ++numFiles;
    }

    // Header
    out.write("PAK", 3);
    out.write(reinterpret_cast<char *>(&version), sizeof(version));
    out.write(reinterpret_cast<char *>(&numFiles), sizeof(numFiles));

    for(auto& entry : fs::recursive_directory_iterator(inputDir)) {
        if(!entry.is_regular_file()) continue;

        std::ifstream in(entry.path(), std::ios::binary);
        std::vector<char> data((std::istreambuf_iterator<char>(in)), {});

        std::string relativePath = fs::relative(entry.path(), inputDir).generic_string();
        uint32_t nameLen = (uint32_t)relativePath.size();
        uint32_t dataLen = (uint32_t)data.size();

        out.write(reinterpret_cast<char *>(&nameLen), sizeof(nameLen));
        out.write(relativePath.data(), nameLen);
        out.write(reinterpret_cast<char *>(&dataLen), sizeof(dataLen));
        out.write(data.data(), dataLen);
    }

    return 0;
}