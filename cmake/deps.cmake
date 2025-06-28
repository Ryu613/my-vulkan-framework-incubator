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

find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
