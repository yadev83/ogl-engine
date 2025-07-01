#include "audiomanager.hpp"

#include "../utils/filesystem.hpp"
#include "../core/logger.hpp"

namespace Engine::Audio {
    ALCdevice *AudioManager::device = nullptr;
    ALCcontext *AudioManager::context = nullptr;

    void AudioManager::Init() {
        LOG_INFO("Opening audio device");
        device = alcOpenDevice(nullptr);
        if(!device) throw std::runtime_error("AudioManager::Init: Could not open audio device");

        LOG_INFO("Creating audio context");
        context = alcCreateContext(device, nullptr);
        if(!context) throw std::runtime_error("AudioManager::Init: Could not create audio context");

        alcMakeContextCurrent(context);
    }

    void AudioManager::Shutdown() {
        LOG_INFO("Destroying audio context");
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        LOG_INFO("Closing audio device");
        alcCloseDevice(device);
    }

    Sound *AudioManager::LoadSound(const std::string& filepath, SoundFileType fileType, SoundType type) {
        Sound *sound = new Sound();
        sound->fileType = fileType;
        sound->type = type;

        LOG_DEBUG("Loading sound");

        switch(sound->fileType) {
            case WAV:
            default: {
                sound->buffer = LoadWAV(filepath);
                break;
            }
        }

        if(sound->buffer == 0) throw std::runtime_error("AudioManager::LoadSound: while loading sound: " + filepath);

        alGenSources(1, &sound->source);
        alSourcei(sound->source, AL_BUFFER, sound->buffer);
        
        return sound;
    }

    Sound *AudioManager::LoadSoundFromMemory(const char *buffer, size_t size, SoundFileType fileType, SoundType type) {
        Sound *sound = new Sound();
        sound->fileType = fileType;
        sound->type = type;

        LOG_DEBUG("Loading sound from memory");

        switch(sound->fileType) {
            case WAV:
            default: {
                sound->buffer = LoadWAVFromMemory(buffer, size);
                break;
            }
        }

        if(sound->buffer == 0) throw std::runtime_error("AudioManager::LoadSoundFromMemory: while loading sound");

        alGenSources(1, &sound->source);
        alSourcei(sound->source, AL_BUFFER, sound->buffer);
        
        return sound;
    }

    void AudioManager::PlaySound(Sound& sound, bool loop, float pitch) {
        alSourcei(sound.source, AL_LOOPING, loop);
        alSourcef(sound.source, AL_PITCH, pitch);
        alSourcef(sound.source, AL_GAIN, sound.volume);

        alSourcePlay(sound.source);
    }

    /** LOADERS */
    ALuint AudioManager::LoadWAVFromMemory(const char *buffer, size_t size) {
        Utils::MemoryStream memStream(buffer, size);

        char riff[4];
        memStream.read(riff, 4);
        if (strncmp(riff, "RIFF", 4) != 0) throw std::runtime_error("AudioManager::LoadWAVFromMemory: ERROR while reading file : not a RIFF file");

        memStream.seek(22);
        short channels;
        memStream.read((char*)&channels, sizeof(short));
        int sampleRate;
        memStream.read((char*)&sampleRate, sizeof(int));
        memStream.seek(34);
        short bitsPerSample;
        memStream.read((char*)&bitsPerSample, sizeof(short));
    
        // Find the datachunk
        char chunkId[4];
        int chunkSize;
        while (true) {
            memStream.read(chunkId, 4);
            memStream.read((char*)&chunkSize, 4);
            if (strncmp(chunkId, "data", 4) == 0) break;
            memStream.seekRelative(chunkSize);
        }
    
        std::vector<char> data(chunkSize);
        memStream.read(data.data(), chunkSize);
    
        ALenum format = 0;
        if (channels == 1 && bitsPerSample == 8) format = AL_FORMAT_MONO8;
        else if (channels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
        else if (channels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
        else if (channels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;
        else {
            throw std::runtime_error("AudioManager::LoadWAVFromMemory: unsupported audio format");
        }

        ALuint out;
        alGenBuffers(1, &out);
        alBufferData(out, format, data.data(), static_cast<ALsizei>(chunkSize), sampleRate);

        return out;
    }

    ALuint AudioManager::LoadWAV(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("AudioManager::LoadWAV: error while opening file");
    
        char riff[4];
        file.read(riff, 4);
        if (strncmp(riff, "RIFF", 4) != 0) throw std::runtime_error("AudioManager::LoadWAV: while reading file : not a RIFF file");

        file.seekg(22);
        short channels;
        file.read((char*)&channels, sizeof(short));
        int sampleRate;
        file.read((char*)&sampleRate, sizeof(int));
        file.seekg(34);
        short bitsPerSample;
        file.read((char*)&bitsPerSample, sizeof(short));
    
        // Find the datachunk
        char chunkId[4];
        int chunkSize;
        while (true) {
            file.read(chunkId, 4);
            file.read((char*)&chunkSize, 4);
            if (strncmp(chunkId, "data", 4) == 0) break;
            file.seekg(chunkSize, std::ios::cur);
        }
    
        std::vector<char> data(chunkSize);
        file.read(data.data(), chunkSize);
    
        ALenum format = 0;
        if (channels == 1 && bitsPerSample == 8) format = AL_FORMAT_MONO8;
        else if (channels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
        else if (channels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
        else if (channels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;
        else {
            throw std::runtime_error("AudioManager::LoadWAVFromMemory: unsupported audio format");
        }

        ALuint buffer;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, format, data.data(), static_cast<ALsizei>(chunkSize), sampleRate);

        return buffer;
    }
}