# Add source to this project's executable.

set(Math_headers
        include/ChaiMath.h
        include/ChaiMathOps.h
        include/ChaiQuat.h
        include/Vec2.h
        include/Vec3.h
        include/Vec4.h
        include/VecBase.h
        include/MatBase.h
        include/Mat_4x4.h
        include/Mat_3x3.h
        include/Mat_2x2.h
        include/MathIncludes.h
)

set(Math_source
        source/ChaiMathOps.cpp
        source/ChaiQuat.cpp
)

add_library(ChaiMath SHARED
        ${Math_headers}
        ${Math_source}
)

target_include_directories(ChaiMath PUBLIC ${CMAKE_BINARY_DIR}/exports/ChaiMath include PRIVATE source/internal)
target_link_libraries(ChaiMath PRIVATE glm::glm)

add_subdirectory(tests)

ChaiLibDefaults(ChaiMath "Core")