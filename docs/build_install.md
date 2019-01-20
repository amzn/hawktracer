# Build and Install {#build_install}
## Downloading source code
Source code is hosted on GitHub servers. To download the code, just run following command:
~~~.sh
git clone https://github.com/loganek/hawktracer.git
~~~
You can also download a tarball for a specific release. All the available releases and their tarballs can be found on [the release page](@repourl/releases).

## Compilation
%HawkTracer uses [CMake](https://cmake.org) for managing the build process. To configure the project and compile it, run following code in the source code directory:
~~~.sh
mkdir build     # create build directory
cd build        # change current directory to 'build'
cmake ..        # configure the project
cmake --build . # compile the project
~~~
It'll configure the project and builds HawkTracer libraries.

Please note that %HawkTracer provides many configuration options, e.g. in order to build static libraries, run:
~~~.sh
cmake .. -DBUILD_STATIC_LIB=ON
~~~
You can access full list of configuration parameters with a description by executing following command in the `build` directory:
~~~.sh
cmake -LH
~~~

## Installing the project
Run following command to install %HawkTracer in your system (you have to build it before, see paragraph above):
~~~.sh
cmake --build . --target install
~~~
By default, CMake installs %HawkTracer to a default location, which is OS-specific (e.g. for most of Linux operating systems it is `/usr/local`). To change the install location, you have to specify `CMAKE_INSTALL_PREFIX` parameter when configuring the project, e.g.:
~~~.sh
cmake .. -DCMAKE_INSTALL_PREFIX=/home/mkolny/ht_install # other configuration parameters here...
~~~
After each re-configuration you usually have to re-compile and re-install the project.

## Using HawkTracer with your project
See page [Integrate HawkTracer to existing project](@ref integration) for information about integrating HawkTracer to your projects.
