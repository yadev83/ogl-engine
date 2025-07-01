#include "filesystem.hpp"

#include <vector>

namespace Engine::Utils {
    std::string LoadFileToString(const std::string& path) {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();

        return ss.str();
    }

    std::vector<char> LoadFileToBuffer(const std::string& path) {
        std::ifstream file("myfile", std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);

        try {
            file.read(buffer.data(), size);
        } catch(std::exception& e) {
            throw std::runtime_error("ERROR while LoadFileToBuffer: " + path);
        }
        
        return buffer;
    }

    MemoryStream::MemoryStream(const char *buffer, size_t size) : mData(buffer), mSize(size), mCur(0) {}

    int MemoryStream::read(void *out, size_t bytesToRead) {
        if(mCur + bytesToRead > mSize) return false;
        std::memcpy(out, mData + mCur, bytesToRead);
        mCur += bytesToRead;

        return true;
    }

    int MemoryStream::seek(size_t offset) {
        if(offset > mSize) return false;
        mCur = offset;
        return true;
    }

    int MemoryStream::seekRelative(size_t offset) {
        if(offset < 0 && size_t(-offset) > mCur) return false;
        if(mCur + offset > mSize) return false;
        mCur += offset;
        return true;
    }

    size_t MemoryStream::tell() const {
        return mCur;
    }

    int MemoryStream::eof() const {
        return mCur >= mSize;
    }
}