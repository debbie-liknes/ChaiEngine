project(Asset)

set(Asset_headers
        include/AssetCache.h
        include/AssetCommon.h
        include/DeferredDeleteQueue.h
        include/Handle.h
        include/ResourceFactory.h
        include/SlotMap.h
)

set(Asset_source
        source/DeferredDeleteQueue.cpp
)

add_library(${PROJECT_NAME}
        ${Asset_headers}
        ${Asset_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC Include)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")