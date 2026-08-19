project(UICore)

set(${PROJECT_NAME}_headers
    include/UI/Core/Buttons.h
    include/UI/Core/Container.h
    include/UI/Core/InternalChaiUI.h
    include/UI/Core/FontManager.h
    include/UI/Core/Layout.h
    include/UI/Core/TreeNode.h
    include/UI/Core/TypeWidgets.h
    include/UI/Core/Widget.h
    include/UI/Core/UIContext.h

    include/UI/Core/Containers/Panel.h

    include/UI/Core/Controls/Label.h
)

set(${PROJECT_NAME}_source
	source/UI/Core/Buttons.cpp
	source/UI/Core/Container.cpp
	source/UI/Core/InternalChaiUI.cpp
	source/UI/Core/FontManager.cpp
	source/UI/Core/Layout.cpp
	source/UI/Core/TreeNode.cpp
	source/UI/Core/TypeWidgets.cpp
	source/UI/Core/Widget.cpp

    source/UI/Core/Containers/Panel.cpp

    source/UI/Core/Controls/Label.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath TypeReflection PRIVATE ImGui Logger Asset Graphics)

ChaiLibDefaults(${PROJECT_NAME} "UI")
