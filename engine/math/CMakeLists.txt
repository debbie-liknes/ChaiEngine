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

add_library(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_headers})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")