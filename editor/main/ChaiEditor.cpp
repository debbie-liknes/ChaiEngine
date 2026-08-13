#include <Runtime/Editor.h>

#include <Tracy/Tracy.hpp>

int main()
{
    // Required to initialize tracy
    TracyNoop;

    using namespace chai;

    Editor editor;

    editor.startup();

    editor.run();

    editor.shutdown();
}
