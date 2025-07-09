# My Vulkan Framework Incubator

2025 startup/template vulkan project for personal learning purpose

## build requirement

1. cmake >4.0
2. vcpkg
3. Vulkan SDK
4. C++20 compiler

### optional

1. editorconfig
1. clang-format

## build

Windows only for now

create `CMakeUserPresets.json` and set specific environment args from `CMakePresets.json`

then you can build

```cmd
// project's root directory
vcpkg install
cmake --preset win-debug-x64
```
