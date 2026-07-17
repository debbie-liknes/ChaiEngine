#include <AudioEngine.h>

int main()
{
    audio::AudioEngine engine;
    engine.init();

    engine.playSound("/path/to/wav", chai::math::Vec3{5, 0, 0});
    while (true)
    {
        engine.update();
    }

    engine.shutdown();

    return 0;
}