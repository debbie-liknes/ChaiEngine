#include <AudioEngine.h>

int main()
{
    AudioEngine engine;
    engine.Init();

    engine.PlaySound("/path/to/wav", chai::Vec3{5, 0, 0});
    while (true)
    {
        engine.Update();
    }

    engine.Shutdown();

    return 0;
}