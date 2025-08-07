include(${PROJECT_ROOT_DIR}/cmake/deps.cmake)

file(GLOB_RECURSE vk1_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp src/*.hpp)

add_library(vk1 STATIC ${vk1_src})
target_link_libraries(vk1
    PUBLIC
        ${Vulkan_LIBRARIES}
        glfw glm::glm-header-only
        tinyobjloader::tinyobjloader
        zetools::utils
        tinygltf
)
target_include_directories(vk1 PUBLIC ${Vulkan_INCLUDE_DIRS} ${Stb_INCLUDE_DIR} include)

add_executable(vktutorial test/vktutorial.cpp)
target_link_libraries(vktutorial 
    PRIVATE 
        ${Vulkan_LIBRARIES}
        glfw glm::glm-header-only
        tinyobjloader::tinyobjloader
)
target_include_directories(vktutorial PRIVATE ${Vulkan_INCLUDE_DIRS})

add_executable(test_vk1 test/test_vk1.cpp)
target_link_libraries(test_vk1 PRIVATE vk1)
