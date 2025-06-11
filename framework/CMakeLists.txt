find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

file(GLOB_RECURSE vodka_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp src/*.hpp)

add_library(vodka STATIC ${vodka_src})
target_link_libraries(vodka PUBLIC ${Vulkan_LIBRARIES} glfw glm::glm-header-only)
target_include_directories(vodka PUBLIC ${Vulkan_INCLUDE_DIRS} include)

add_executable(main test/main.cpp)
target_link_libraries(main PRIVATE ${Vulkan_LIBRARIES} glfw glm::glm-header-only)
target_include_directories(main PRIVATE ${Vulkan_INCLUDE_DIRS} include)
