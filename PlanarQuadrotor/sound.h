#include <string>
#include <SDL.h> 

class Sound {
public:
    Sound(std::string path);
    ~Sound();
    void Tool();
    void Go();
    void End();
    int Playing();
private:
    SDL_AudioDeviceID dev;
    SDL_AudioSpec spec;
    Uint8* audio_buf;
    Uint32        audio_len;
};