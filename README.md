# My Vulkan Framework Incubator

vulkan startup/template project for learning purpose

## building requirement

1. cmake 4.0+
1. vcpkg
1. Vulkan SDK

### optional requirement

1. editorconfig support
1. clang-format support

## build

now this project is for Windows

alter `CMakeUserPrests.json` to your directory of `c/c++ compilers` and `vcpkg toolchain`, then

```cmd
// project's root directory
vcpkg install
cmake --preset win-debug-x64
```
