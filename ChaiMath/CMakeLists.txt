# Add source to this project's executable.

set(Math_headers
	include/ChaiMath.h
	include/ChaiMathOps.h
	include/ChaiMatrix.h
	include/ChaiQuat.h
	include/ChaiVector.h
	include/VecBase.h
	include/VecTraits.h
)

set(MATH_private
	source/internal/glm_internal.h
)

set(Math_source
	source/ChaiMathOps.cpp
	source/VecBase.cpp
	source/ChaiMatrix.cpp
	source/ChaiQuat.cpp
)

add_library(ChaiMath SHARED 
	${Math_headers} 
	${Math_source}
	${MATH_private}
)

target_include_directories(ChaiMath PUBLIC ${CMAKE_BINARY_DIR}/exports/ChaiMath include PRIVATE source/internal)
target_link_libraries(ChaiMath PRIVATE glm::glm)


ChaiLibDefaults(ChaiMath "Core")