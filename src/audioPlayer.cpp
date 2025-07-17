#include "audioPlayer.hpp"
#include <iostream>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

static ma_engine engine;

void AudioPlayer::init() {
    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
    }
}

void AudioPlayer::play(const std::string& filename, float volume) {
    // Check if the sound is already loaded
    if (sounds.contains(filename)) {
        ma_sound* s = sounds[filename].get();
        if (!ma_sound_is_playing(s)) {
            ma_sound_start(s);
            ma_sound_set_volume(s, volume);
        }
        return;
    }

    // Load the sound file
    auto sound = std::make_unique<ma_sound>();
    ma_result result = ma_sound_init_from_file(&engine, filename.c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, nullptr, sound.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return;
    }

    // Set volume and start playing
    ma_sound_set_volume(sound.get(), volume);
    ma_sound_start(sound.get());
    sounds[filename] = std::move(sound);
}

void AudioPlayer::stop(const std::string& filename) {
    if (sounds.contains(filename)) {
        ma_sound_stop(sounds[filename].get());
    }
}

bool AudioPlayer::isPlaying(const std::string& filename) const {
    if (sounds.contains(filename)) {
        return ma_sound_is_playing(sounds.at(filename).get());
    }
    return false;
}
