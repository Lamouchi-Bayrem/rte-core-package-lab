I’ll explain exactly what RTE means in this project, what files I generated, and what happens from the moment you run CMake until the final executable starts. The most important clarification is that this project is an RTE-style learning example, not a complete AUTOSAR RTE implementation.

1. What does RTE mean?

RTE means:

Plain Text
1
Runtime Environment
Show more lines

A Runtime Environment is a software layer that allows application components to interact with:

Other software components
Hardware drivers
Operating-system services
Communication services
Diagnostic services
Memory services
Sensors and actuators

Conceptually:

Plain Text
1
Application
2
|
3
v
4
Runtime Environment
5
|
6
v
7
Drivers / OS / Hardware
Show more lines

The RTE separates the application logic from the lower-level implementation.

For example, the application could call:

C
1
Rte_Read_Temperature(&temperature);
Show more lines

The application does not need to know whether the temperature comes from:

An ADC
An I2C sensor
A CAN message
Shared memory
Another software component

The RTE handles that connection.

2. Important clarification about our project

The project I generated is not a complete AUTOSAR RTE.

It is a small RTE-style reusable library designed to teach you:

Modular C architecture
Public header files
Static libraries
CMake target management
Package installation
CMake package configuration
pkg-config
Unit testing
Optional features
Yocto integration
Consumer and producer separation

In the example, the RTE library contains only a simple counter module:

C
1
rte_counter_init();
2
rte_counter_increment();
3
rte_counter_get();
Show more lines

The counter is not the real purpose. It represents a reusable embedded service.

Later, you could replace the counter with real embedded modules:

Plain Text
1
RTE GPIO service
2
RTE UART service
3
RTE CAN service
4
RTE diagnostic service
5
RTE configuration service
6
RTE sensor service
7
RTE logging service
Show more lines
3. What did I generate?

The project contains four main parts:

Plain Text
1
embedded_package_config_lab/
2
├── rte-core/
3
├── consumer-cmake/
4
├── consumer-pkgconfig/
5
└── yocto/
Show more lines

Each folder has a different responsibility.

4. The producer library: rte-core

The rte-core folder is the producer.

It produces a reusable static library:

Plain Text
1
librte_core.a
Show more lines

Its structure is:

Plain Text
1
rte-core/
2
├── CMakeLists.txt
3
├── include/
4
│ └── rte/
5
│ └── counter.h
6
├── src/
7
│ └── counter.c
8
├── tests/
9
│ └── test_counter.c
10
└── cmake/
11
├── RteCoreConfig.cmake.in
12
└── rte-core.pc.in
Show more lines
Public header

The public header is:

Plain Text
1
include/rte/counter.h
Show more lines

It declares the interface:

C
1
typedef struct
2
{
3
uint32_t value;
4
} rte_counter_t;
5
 
6
void rte_counter_init(rte_counter_t *counter);
7
void rte_counter_increment(rte_counter_t *counter);
8
uint32_t rte_counter_get(const rte_counter_t *counter);
Show more lines

This tells other source files:

These functions and this data type exist.

The header does not normally contain the compiled implementation.

Source implementation

The implementation is located in:

Plain Text
1
src/counter.c
Show more lines

It defines what the functions actually do:

C
1
void rte_counter_init(rte_counter_t *counter)
2
{
3
if (counter != NULL)
4
{
5
counter->value = 0U;
6
}
7
}
Show more lines

For example:

C
1
rte_counter_init(&counter);
Show more lines

sets the counter value to zero.

The function:

C
1
rte_counter_increment(&counter);
Show more lines

increments the value:

Plain Text
1
0 → 1 → 2 → 3
Show more lines

The function:

C
1
rte_counter_get(&counter);
Show more lines

returns the current value.

5. The CMake consumer

The folder:

Plain Text
1
consumer-cmake/
Show more lines

represents a completely separate application.

It does not compile counter.c directly. Instead, it searches for an already installed RTE library:

CMake
1
find_package(RteCore 1.2 REQUIRED CONFIG)
Show more lines

Then it links against the imported target:

CMake
1
target_link_libraries(
2
rte_consumer
3
PRIVATE
4
RteCore::rte_core
5
)
Show more lines

This is professional dependency management.

The consumer does not need to manually write:

CMake
1
include_directories(/some/path/include)
2
link_directories(/some/path/lib)
Show more lines

It also does not need to hardcode:

Plain Text
1
/some/path/lib/librte_core.a
Show more lines

The imported target:

CMake
1
RteCore::rte_core
Show more lines

already contains the necessary information.

6. The pkg-config consumer

The second consumer is:

Plain Text
1
consumer-pkgconfig/
Show more lines

It discovers the library using:

Plain Text
1
rte-core.pc
Show more lines

The .pc file contains metadata such as:

Plain Text
1
Name: rte-core
2
Version: 1.2.0
3
Cflags: -I${includedir}
4
Libs: -L${libdir} -lrte_core
Show more lines

You can inspect it using:

Shell
1
pkg-config --cflags --libs rte-core
Show more lines

It may produce something similar to:

Plain Text
1
-I/path/to/stage/include \
2
-L/path/to/stage/lib \
3
-lrte_core
Show more lines

These are the flags required by GCC to compile and link the application.

7. The Yocto part

The project also contains an educational Yocto recipe:

Plain Text
1
rte-core_1.2.0.bb
Show more lines

Its purpose is to show how the same library could be built for an Embedded Linux target.

The recipe contains:

Plain Text
bitbake isn’t fully supported. Syntax highlighting is based on Plain Text.
1
inherit cmake pkgconfig
Show more lines

This tells Yocto:

The project uses CMake
The project provides or consumes pkg-config metadata
Yocto must prepare the cross-compilation environment correctly

It also contains:

Plain Text
bitbake isn’t fully supported. Syntax highlighting is based on Plain Text.
1
PACKAGECONFIG[trace] = \
2
"-DRTE_ENABLE_TRACE=ON,-DRTE_ENABLE_TRACE=OFF,,"
Show more lines

This connects a Yocto feature to a CMake option.

When trace is enabled in Yocto:

Plain Text
bitbake isn’t fully supported. Syntax highlighting is based on Plain Text.
1
PACKAGECONFIG:append:pn-rte-core = " trace"
Show more lines

Yocto sends this to CMake:

Shell
1
-DRTE_ENABLE_TRACE=ON
Show more lines
8. What happens when you run CMake?

Suppose you execute:

Shell
1
cmake -S rte-core -B build/rte-core \
2
-DCMAKE_BUILD_TYPE=Debug \
3
-DCMAKE_INSTALL_PREFIX="$PWD/stage"
Show more lines

CMake does not compile the C code yet.

It performs the configuration and generation stages.

Step 1: Read CMakeLists.txt

CMake reads:

Plain Text
1
rte-core/CMakeLists.txt
Show more lines

It finds the project definition:

CMake
1
project(RteCore VERSION 1.2.0 LANGUAGES C)
Show more lines

Now CMake knows:

Project name: RteCore
Version: 1.2.0
Language: C
Step 2: Detect the compiler

CMake searches for a C compiler, normally:

Plain Text
1
gcc
Show more lines

It checks:

Can GCC compile a simple C program?
What GCC version is installed?
What platform is being used?
What object-file format is required?
What linker should be used?

For cross-compilation, it might instead use:

Plain Text
1
arm-none-eabi-gcc
Show more lines

or:

Plain Text
1
aarch64-linux-gnu-gcc
Show more lines
Step 3: Process the library target

CMake reads:

CMake
1
add_library(
2
rte_core
3
STATIC
4
src/counter.c
5
)
Show more lines

This means:

Create a static library named rte_core from counter.c.

On Linux, the result is normally:

Plain Text
1
librte_core.a
Show more lines

The naming is:

Plain Text
1
lib + rte_core + .a
Show more lines
Step 4: Process include directories

CMake reads:

CMake
1
target_include_directories(
2
rte_core
3
PUBLIC
4
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
5
$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
6
)
Show more lines

During the local build, the compiler receives something similar to:

Shell
1
-I/path/to/project/rte-core/include
Show more lines

This allows:

C
1
#include "rte/counter.h"
Show more lines

to work.

After installation, consumers use the installed include directory instead:

Plain Text
1
stage/include/
Show more lines
Step 5: Process compiler warnings

CMake adds:

Plain Text
1
-Wall
2
-Wextra
3
-Wpedantic
Show more lines

The approximate compiler command becomes:

Shell
1
gcc \
2
-std=c11 \
3
-Wall \
4
-Wextra \
5
-Wpedantic \
6
-I/path/to/rte-core/include \
7
-c src/counter.c \
8
-o counter.c.o
Show more lines
9. What happens when you build?

You run:

Shell
1
cmake --build build/rte-core --parallel
Show more lines

Now the real C compilation begins.

Stage 1: Preprocessing

The C preprocessor handles:

C
1
#include "rte/counter.h"
2
#include <stddef.h>
Show more lines

It also processes macros:

C
1
#ifdef RTE_ENABLE_TRACE
Show more lines

If tracing is disabled, this code is excluded.

If tracing is enabled:

Shell
1
-DRTE_ENABLE_TRACE
Show more lines

the trace code is included.

The conceptual result is one expanded translation unit:

Plain Text
1
counter.c
2
+ counter.h
3
+ standard headers
4
+ macro expansion
5
= preprocessed C source
Show more lines

You can manually inspect this stage with:

Shell
1
gcc -E src/counter.c \
2
-Iinclude \
3
-o counter.i
Show more lines

The generated file is:

Plain Text
1
counter.i
Show more lines
Stage 2: Compilation

GCC translates the preprocessed C code into assembly instructions.

Conceptually:

Plain Text
1
counter.i
2
↓
3
C compiler
4
↓
5
counter.s
Show more lines

You can inspect this manually:

Shell
1
gcc -S counter.i -o counter.s
Show more lines

The counter.s file contains assembly instructions for your processor.

On your PC, that may be x86-64 assembly.

For an STM32, it would be ARM Cortex-M assembly.

Stage 3: Assembly

The assembler converts assembly instructions into machine code:

Plain Text
1
counter.s
2
↓
3
Assembler
4
↓
5
counter.o
Show more lines

The .o file is an object file.

It contains:

Machine code
Symbols
Relocation information
Debug information in Debug builds
References to functions that may not yet be resolved

You can create it manually:

Shell
1
gcc -c src/counter.c \
2
-Iinclude \
3
-o counter.o
Show more lines
Stage 4: Static library creation

Because the CMake target is:

CMake
1
add_library(rte_core STATIC ...)
Show more lines

the object file is placed into a static archive:

Plain Text
1
counter.o
2
↓
3
Archiver
4
↓
5
librte_core.a
Show more lines

Conceptually, CMake runs something similar to:

Shell
1
ar rcs librte_core.a counter.o
Show more lines

At this point, the static library exists, but it is not an executable.

You cannot directly run:

Shell
1
./librte_core.a
Show more lines

because it does not contain an application entry point such as:

C
1
int main(void)
Show more lines
10. What happens when the consumer is compiled?

The consumer contains:

C
1
int main(void)
2
{
3
rte_counter_t counter;
4
 
5
rte_counter_init(&counter);
6
 
7
for (unsigned int i = 0U; i < 3U; ++i)
8
{
9
rte_counter_increment(&counter);
10
}
11
 
12
printf("Counter = %u\n", rte_counter_get(&counter));
13
 
14
return 0;
15
}
Show more lines

The consumer source is compiled:

Plain Text
1
main.c
2
↓
3
main.o
Show more lines

Then the linker combines:

Plain Text
1
main.o
2
+
3
librte_core.a
4
+
5
C standard library
6
=
7
rte_consumer executable
Show more lines

The conceptual linker command is:

Shell
1
gcc main.o \
2
-L/path/to/stage/lib \
3
-lrte_core \
4
-o rte_consumer
Show more lines
11. What does the linker do?

The consumer object file contains unresolved references such as:

Plain Text
1
rte_counter_init
2
rte_counter_increment
3
rte_counter_get
4
printf
Show more lines

The linker searches for their implementations.

It finds:

Plain Text
1
rte_counter_init
2
rte_counter_increment
3
rte_counter_get
Show more lines

inside:

Plain Text
1
librte_core.a
Show more lines

It finds:

Plain Text
1
printf
Show more lines

inside the C runtime and standard C library.

The linker then produces:

Plain Text
1
rte_consumer
Show more lines

If you forget to link rte_core, you receive errors similar to:

Plain Text
1
undefined reference to `rte_counter_init'
2
undefined reference to `rte_counter_increment'
3
undefined reference to `rte_counter_get'
Show more lines

That means:

The compiler understood the declarations from the header, but the linker could not find the implementations.

12. What happens when the program runs?

When you execute:

Shell
1
./build/consumer-cmake/rte_consumer
Show more lines

the operating system loads the executable into memory.

Execution begins at the program startup code, which eventually calls:

C
1
main()
Show more lines

Inside main():

First operation
C
1
rte_counter_init(&counter);
Show more lines

Result:

Plain Text
1
counter.value = 0
Show more lines
First increment
C
1
rte_counter_increment(&counter);
Show more lines

Result:

Plain Text
1
counter.value = 1
Show more lines
Second increment

Result:

Plain Text
1
counter.value = 2
Show more lines
Third increment

Result:

Plain Text
1
counter.value = 3
Show more lines
Read the value
C
1
rte_counter_get(&counter);
Show more lines

Returns:

Plain Text
1
3
Show more lines

The program displays:

Plain Text
1
CMake consumer counter=3
Show more lines

If tracing is enabled, it also calls:

C
1
rte_counter_trace(&counter);
Show more lines

and displays:

Plain Text
1
[RTE TRACE] counter=3
Show more lines
13. What happens during installation?

You run:

Shell
1
cmake --install build/rte-core
Show more lines

CMake copies the development artifacts into the installation prefix.

The result is:

Plain Text
1
stage/
2
├── include/
3
│ └── rte/
4
│ └── counter.h
5
│
6
└── lib/
7
├── librte_core.a
8
├── cmake/
9
│ └── RteCore/
10
│ ├── RteCoreConfig.cmake
11
│ ├── RteCoreConfigVersion.cmake
12
│ └── RteCoreTargets.cmake
13
│
14
└── pkgconfig/
15
└── rte-core.pc
Show more lines

Each file has a role:

counter.h: public C API
librte_core.a: compiled implementation
RteCoreConfig.cmake: allows CMake consumers to find the package
RteCoreConfigVersion.cmake: validates requested versions
RteCoreTargets.cmake: defines RteCore::rte_core
rte-core.pc: provides GCC compile and linker flags through pkg-config
14. Complete flow
Plain Text
1
counter.h
2
Public declarations
3
+
4
counter.c
5
Function implementations
6
|
7
v
8
Preprocessor
9
Expands headers and macros
10
|
11
v
12
Compiler
13
Generates assembly
14
|
15
v
16
Assembler
17
Generates counter.o
18
|
19
v
20
Archiver
21
Generates librte_core.a
22
|
23
v
24
CMake install
25
Installs headers, library, and metadata
26
|
27
v
28
Consumer find_package()
29
Finds RteCore::rte_core
30
|
31
v
32
Consumer main.c compiled into main.o
33
|
34
v
35
Linker
36
Combines main.o and librte_core.a
37
|
38
v
39
rte_consumer executable
40
|
41
v
42
Program starts and calls main()
Show more lines
The main idea to remember

The current example uses RTE as the name of a reusable runtime-oriented library, not as a complete automotive runtime generator.

The professional concept demonstrated is:

Plain Text
1
Build a library once
2
↓
3
Install the library and its public API
4
↓
5
Describe it with package metadata
6
↓
7
Let independent applications discover it
8
↓
9
Compile and link without hardcoded paths
Show more lines

That separation is very important in Embedded Linux, SDK development, automotive middleware, and large modular C/C++ systems.
