//
// Created by Tim Müller on 13.07.25.
//

#ifndef ARCADE_AUDIOPLAYER_HPP
#define ARCADE_AUDIOPLAYER_HPP

#include <iostream>
#include <memory>
#include <unordered_map>
#include "miniaudio.h"


class AudioPlayer {
public:
    void init();
    void play(const std::string& filename, float volume = 1.0f);
    void stop(const std::string& filename);
    bool isPlaying(const std::string& filename) const;

private:
    ma_engine engine;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> sounds;
};



#endif //ARCADE_AUDIOPLAYER_HPP
