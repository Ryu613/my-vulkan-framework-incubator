include(FetchContent)

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
