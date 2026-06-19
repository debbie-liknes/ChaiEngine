project(Asset)

set(${PROJECT_NAME}_headers
        include/AssetCache.h
        include/AssetCommon.h
        include/DeferredDeleteQueue.h
        include/Handle.h
        include/ResourceFactory.h
        include/SlotMap.h
)

set(${PROJECT_NAME}_source
        source/DeferredDeleteQueue.cpp
)

add_library(${PROJECT_NAME}
        ${Asset_headers}
        ${Asset_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")