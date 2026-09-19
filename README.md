# Embedded Package Configuration Lab

## Goal

This lab shows how an embedded library is built once, installed as a development package, discovered by independent applications, turned into an archive, and described to Yocto/OpenEmbedded.

It covers four mechanisms that are often confused:

1. **CMake package configuration**: `RteCoreConfig.cmake`, version file, and exported targets used by `find_package(RteCore CONFIG)`.
2. **pkg-config**: `rte-core.pc`, which publishes compiler and linker flags for Linux tooling.
3. **CPack**: creates a distributable archive from CMake install rules.
4. **Yocto PACKAGECONFIG**: enables or disables optional recipe features. It is not the same as a CMake package config file.

## Why this matters in embedded systems

For bare-metal firmware, internal modules are often built in one tree with `add_subdirectory()`. A package configuration becomes valuable when a HAL, middleware stack, protocol library, diagnostic component, or vendor SDK must be reused by several independent applications.

For Embedded Linux, libraries are normally cross-compiled into a sysroot. The sysroot contains headers, libraries, CMake config files, and `.pc` metadata. Applications discover dependencies from that target sysroot instead of accidentally using host `/usr/include` or `/usr/lib` files.

## Architecture

```text
rte-core source
  |-- builds librte_core.a
  |-- installs rte/counter.h
  |-- exports RteCore::rte_core
  |-- installs RteCoreConfig.cmake
  |-- installs rte-core.pc
  `-- CPack creates rte-core-1.2.0-Linux.tar.gz

consumer-cmake    -> find_package(RteCore) -> RteCore::rte_core
consumer-pkgconfig -> pkg_check_modules()  -> PkgConfig::RTE_CORE
Yocto recipe      -> inherit cmake pkgconfig + PACKAGECONFIG[trace]
```

## Project tree

```text
embedded_package_config_lab/
├── rte-core/                 # Producer library
├── consumer-cmake/           # Consumes exported CMake target
├── consumer-pkgconfig/       # Consumes .pc metadata
└── yocto/                    # Educational recipe and local.conf example
```

## Lab A: build, test, and install the producer

From the lab root:

```bash
cmake -S rte-core -B build/rte-core \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX="$PWD/stage"
cmake --build build/rte-core --parallel
ctest --test-dir build/rte-core --output-on-failure
cmake --install build/rte-core
```

The install tree should resemble:

```text
stage/
├── include/rte/counter.h
├── lib/librte_core.a
├── lib/cmake/RteCore/
│   ├── RteCoreConfig.cmake
│   ├── RteCoreConfigVersion.cmake
│   └── RteCoreTargets*.cmake
└── lib/pkgconfig/rte-core.pc
```

## Lab B: consume with CMake package config

```bash
cmake -S consumer-cmake -B build/consumer-cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$PWD/stage"
cmake --build build/consumer-cmake --parallel
./build/consumer-cmake/rte_consumer
```

Key producer-side commands:

```cmake
install(TARGETS rte_core EXPORT RteCoreTargets ...)
install(EXPORT RteCoreTargets NAMESPACE RteCore:: ...)
configure_package_config_file(...)
write_basic_package_version_file(...)
```

Key consumer-side commands:

```cmake
find_package(RteCore 1.2 REQUIRED CONFIG)
target_link_libraries(rte_consumer PRIVATE RteCore::rte_core)
```

The imported target carries the library path, include directory, and public compile definitions. The consumer does not hardcode `-I`, `-L`, or an absolute library path.

## Lab C: consume with pkg-config

```bash
export PKG_CONFIG_PATH="$PWD/stage/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
pkg-config --modversion rte-core
pkg-config --cflags --libs rte-core
cmake -S consumer-pkgconfig -B build/consumer-pkgconfig
cmake --build build/consumer-pkgconfig --parallel
./build/consumer-pkgconfig/rte_pc_consumer
```

Direct GCC usage is also possible:

```bash
gcc consumer-pkgconfig/src/main.c \
  $(pkg-config --cflags --libs rte-core) \
  -o build/rte_pc_direct
```

For cross-compilation, the SDK or build system should set sysroot-aware pkg-config variables. Do not point target builds at the host package database.

## Lab D: optional package feature

Enable tracing in the producer:

```bash
cmake -S rte-core -B build/rte-core-trace \
  -DRTE_ENABLE_TRACE=ON \
  -DCMAKE_INSTALL_PREFIX="$PWD/stage-trace"
cmake --build build/rte-core-trace --parallel
cmake --install build/rte-core-trace
```

Because `RTE_ENABLE_TRACE` is a public usage requirement, a CMake consumer linking `RteCore::rte_core` sees the matching declaration automatically.

## Lab E: create a distributable package with CPack

```bash
cmake -S rte-core -B build/package -DCMAKE_BUILD_TYPE=Release
cmake --build build/package --parallel
cpack --config build/package/CPackConfig.cmake
```

CPack uses the `install()` rules. This example generates a `.tar.gz` archive, not a Debian or RPM package.

## Yocto/OpenEmbedded integration

The sample recipe contains:

```bitbake
inherit cmake pkgconfig
PACKAGECONFIG[trace] = "-DRTE_ENABLE_TRACE=ON,-DRTE_ENABLE_TRACE=OFF,,"
```

Enable the feature with:

```bitbake
PACKAGECONFIG:append:pn-rte-core = " trace"
```

Important distinction:

- `PACKAGECONFIG` controls recipe build features.
- `PACKAGES` controls output package splitting.
- `IMAGE_INSTALL` decides which runtime packages enter the image.
- `RteCoreConfig.cmake` helps another CMake project locate the installed development files.
- `rte-core.pc` supplies compile/link metadata to pkg-config consumers.

A real recipe should fetch a source archive or Git revision and use a verified checksum. The included recipe is an educational template and expects `rte-core-1.2.0.tar.gz` in its `files` directory.

## CMake package cheat sheet

### Build-tree dependency

```cmake
add_subdirectory(path/to/rte-core)
target_link_libraries(app PRIVATE RteCore::rte_core)
```

Use when producer and consumer are part of one source tree.

### Installed package dependency

```cmake
find_package(RteCore REQUIRED CONFIG)
target_link_libraries(app PRIVATE RteCore::rte_core)
```

Use when the library is independently installed or delivered in an SDK/sysroot.

### Search hints

```bash
cmake -S consumer-cmake -B build/consumer \
  -DCMAKE_PREFIX_PATH=/path/to/prefix
```

Or point directly to the config directory:

```bash
cmake -S consumer-cmake -B build/consumer \
  -DRteCore_DIR=/path/to/lib/cmake/RteCore
```

### Package version compatibility

```cmake
write_basic_package_version_file(
  RteCoreConfigVersion.cmake
  VERSION 1.2.0
  COMPATIBILITY SameMajorVersion)
```

A consumer requesting `1.2` can accept later compatible `1.x` versions, while `2.x` is rejected under this policy.

### Relocatable config

Use `configure_package_config_file()` instead of plain `configure_file()` for installed CMake package configs. Combine build and install interfaces:

```cmake
target_include_directories(rte_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
```

### Debug commands

```bash
cmake --build build/rte-core --verbose
cmake --build build/rte-core --target help
cmake --install build/rte-core --prefix "$PWD/alternative-stage"
cmake --find-package -DNAME=RteCore -DCOMPILER_ID=GNU -DLANGUAGE=C -DMODE=EXIST
```

### Clean rebuild

```bash
rm -rf build stage stage-trace
```

## Professional lessons demonstrated

- Target-based CMake
- Static library design
- Public headers and API namespace
- Unit testing with CTest
- Install-tree design with GNUInstallDirs
- Export sets and namespaced imported targets
- Relocatable package configuration
- Semantic version checks
- pkg-config metadata generation
- Optional compile features
- CPack archive generation
- Yocto recipe integration and feature control
- Separation between producer and consumer projects
