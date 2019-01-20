# Integrate %HawkTracer to existing project {#integration}

%HawkTracer is a library which provides functions for instrumenting the code. To provide best user experience, %HawkTracer supports multiple build systems. If you can't find the one you use, please [let us know](https://github.com/loganek/hawktracer/issues/new).
Following build systems/tools are already supported:
* [CMake](#CMake)
* [pkg-config](#pkg-config)

## CMake
There are 2 ways of integrating %HawkTracer with CMake project; if %HawkTracer is already installed in your system, you can just include `%HawkTracer` config file and use exported targets. If you don't want to download and build %HawkTracer as a part of build process of your project, we recommend to use CMake `ExternalProject` module.

### ExternalProject
You can find example project which uses CMake `ExternalProject` module to include %HawkTracer in the [repository](@repocodeurl/examples/integrations/cmake-external-project).
You can copy the [hawktracer.cmake](@repocodeurl/examples/integrations/cmake-external-project/hawktracer.cmake) file to your project, include it in one of `CMakeLists.txt` files, and link to `%hawktracer` target:
```cmake
# include cmake file
include(hawktracer.cmake)

# define your target
add_executable(your_project main.cpp)
# link hawktracer to your target
target_link_libraries(your_project hawktracer)
```

### %HawkTracer module
If you have %HawkTracer installed in your system, you can simply include it to your project:
```cmake
# include HawkTracer package, version is not required
find_package(HawkTracer 0.5.0 REQUIRED)

# define your target
add_executable(test main.cc)
# link hawktracer to your target
target_link_libraries(test HawkTracer::hawktracer)
```
The full example can be found in the [repository](@repocodeurl/examples/integrations/cmake-find-package).

## pkg-config
%HawkTracer package includes a **pkg-config** file. In order to use it, %HawkTracer has to be installed in your system. The simplest usage:
```sh
gcc my_file.c $(pkg-config --cflags --libs hawktracer)
```
If %HawkTracer module can't be found, you should set `PKG_CONFIG_PATH` to a directory which contains `hawktracer.pc` file (it should be in installation directory, in a `share/pkgconfig` subdirectory):
```sh
export PKG_CONFIG_PATH=/path/to/hawktracer/install_dir/share/pkgconfig
gcc my_file.c $(pkg-config --cflags --libs hawktracer)
```
