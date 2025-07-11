include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# ze-tools
FetchContent_Declare(
	zetools
	GIT_REPOSITORY https://github.com/Ryu613/ze-tools.git
	GIT_TAG origin/master
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	SOURCE_DIR ${PROJECT_ROOT_DIR}/deps/zetools
)
FetchContent_MakeAvailable(zetools)

# tinygltf
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
FetchContent_Declare(
	tinygltf
	GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
	GIT_TAG 37250b3470b517fd6823ee82d6e0495695bb7924 #v2.9.6
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	SOURCE_DIR ${PROJECT_ROOT_DIR}/deps/tinygltf
)
FetchContent_MakeAvailable(tinygltf)

find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
