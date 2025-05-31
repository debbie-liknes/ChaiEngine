# External submodule libraries

add_subdirectory(glm)
add_subdirectory(glfw)
add_subdirectory(glad/cmake)

# GLAD requires additional config to create a library with
# a specific name (in this case, `glad`).
# gl:core= specifies the core API and assumes latest
glad_add_library(glad REPRODUCIBLE API gl:core=)
