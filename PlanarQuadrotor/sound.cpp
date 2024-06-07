#include <string>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include "sound.h"

Sound::Sound(std::string path) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    std::string fullPath = path; // Adjust path if necessary

    // Check if file exists
    std::ifstream file(fullPath);
    if (!file) {
        std::cerr << "WAV file not found: " << fullPath << std::endl;
        return;
    }

    if (SDL_LoadWAV(fullPath.c_str(), &spec, &audio_buf, &audio_len) == nullptr) {
        std::cerr << "Failed to load WAV file: " << SDL_GetError() << std::endl;
        return;
    }

    // Print original audio spec for debugging
    std::cout << "Original audio spec - freq: " << spec.freq << ", format: " << spec.format << ", channels: " << (int)spec.channels << std::endl;

    dev = 0; // Initialize dev to an invalid value
}

Sound::~Sound() {
    if (audio_buf != nullptr) {
        SDL_FreeWAV(audio_buf);
    }
    if (dev != 0) {
        SDL_CloseAudioDevice(dev);
    }
    SDL_Quit();
}

void Sound::Tool() {
    // Use the original spec as the desired spec
    SDL_AudioSpec obtainedSpec;
    dev = SDL_OpenAudioDevice(nullptr, 0, &spec, &obtainedSpec, 0);
    if (dev == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
    }
    else {
        std::cout << "Audio device opened successfully. Device ID: " << dev << std::endl;
        std::cout << "Obtained audio spec - freq: " << obtainedSpec.freq << ", format: " << obtainedSpec.format << ", channels: " << (int)obtainedSpec.channels << std::endl;
    }
}

void Sound::Go() {
    if (dev != 0) {
        std::cout << "Queuing audio buffer of length: " << audio_len << std::endl;
        int status = SDL_QueueAudio(dev, audio_buf, audio_len);
        if (status < 0) {
            std::cerr << "Failed to queue audio: " << SDL_GetError() << std::endl;
        }
        else {
            std::cout << "Audio queued successfully, starting playback." << std::endl;
            SDL_PauseAudioDevice(dev, 0);

            // Check the device status after starting playback
            SDL_AudioStatus audioStatus = SDL_GetAudioDeviceStatus(dev);
            if (audioStatus == SDL_AUDIO_PLAYING) {
                std::cout << "Audio is playing." << std::endl;
            }
            else if (audioStatus == SDL_AUDIO_PAUSED) {
                std::cout << "Audio is paused." << std::endl;
            }
            else {
                std::cout << "Audio is stopped." << std::endl;
            }
        }
    }
    else {
        std::cerr << "Invalid audio device ID, cannot play audio" << std::endl;
    }
}

void Sound::End() {
    if (dev != 0) {
        SDL_PauseAudioDevice(dev, 1);
    }
    else {
        std::cerr << "Invalid audio device ID, cannot pause audio" << std::endl;
    }
}

int Sound::Playing() {
    if (dev != 0) {
        return SDL_GetAudioDeviceStatus(dev);
    }
    return 0;
}
