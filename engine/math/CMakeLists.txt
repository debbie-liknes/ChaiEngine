project(ChaiMath)

set(${PROJECT_NAME}_headers
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

set(${PROJECT_NAME}_source
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/exports/${PROJECT_NAME} include PRIVATE source/internal)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")