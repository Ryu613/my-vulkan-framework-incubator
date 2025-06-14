# My Vulkan Framework Incubator

vulkan startup/template project for learning purpose

## building requirement

1. cmake 4.0+
2. vcpkg
3. Vulkan SDK
4. C++20 compiler

### optional requirement

1. editorconfig support
1. clang-format support

## build

now this project is for Windows

alter `CMakeUserPrests.json` to your directory of `c++ compilers` and `vcpkg toolchain`, then

```cmd
// project's root directory
vcpkg install
cmake --preset win-debug-x64
```
