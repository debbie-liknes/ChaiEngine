#include <Engine.h>
#include <SpdLogSink.h>
#include <Log.h>

int main()
{
    chai::SpdlogSink logSink; 
    chai::setLogSink(&logSink);
    chai::setLogLevel(chai::LogLevel::Info);

    chai::Engine engine;
    engine.startup();
    engine.tick();
    engine.shutdown();
}