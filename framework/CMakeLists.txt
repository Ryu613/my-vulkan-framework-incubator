find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

include(${PROJECT_ROOT_DIR}/cmake/deps.cmake)

file(GLOB_RECURSE vk1_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp src/*.hpp)

add_library(vk1 STATIC ${vk1_src})
target_link_libraries(vk1 PUBLIC ${Vulkan_LIBRARIES} glfw glm::glm-header-only zetools::utils)
target_include_directories(vk1 PUBLIC ${Vulkan_INCLUDE_DIRS} include)

add_executable(main test/main.cpp)
target_link_libraries(main PRIVATE ${Vulkan_LIBRARIES} glfw glm::glm-header-only)
target_include_directories(main PRIVATE ${Vulkan_INCLUDE_DIRS} include)

add_executable(test_vk1 test/test_vk1.cpp)
target_link_libraries(test_vk1 PRIVATE vk1)
