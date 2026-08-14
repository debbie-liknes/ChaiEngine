#include <Runtime/Editor.h>

#include <tracy/Tracy.hpp>

int main()
{
    // Required to initialize tracy
    TracyNoop;

    using namespace chai;

    Editor editor;

    if (!editor.startup())
        return 1;

    editor.run();

    editor.shutdown();

    return 0;
}
