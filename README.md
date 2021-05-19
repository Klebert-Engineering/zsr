```sh
  ################################################
  ##              ###                            ##
  ##             ##                              ##
  #######       ##      ##       ##     ###     ##
      ##       ##        #########     ##      ##
     ##       ##               ##           ###
    ##        #######         ##           ##
   ##               ##       ##             ##
  ##                       ###      ###      ##
  ##################################  #########

     Z S E R I O  C + +  R E F L E C T I O N
```

# About

Zsr provides a c++ runtime reflection layer for [zserio](https://github.com/ndsev/zserio) generated c++ modules.

# Building

```sh
mkdir build && cd build
cmake -DZSERIO_RT_DIR=<path/to/zserio/compiler/extensions/cpp/runtime/src> ..
cmake --build .
```

```sh
ctest .
```

# Usage

## CMake

In your project CMakeLists.txt, call the `add_zserio_module` function to generate and
build a zserio c++ module with reflection metadata. Assuming zswag is a Git submodule of your project,
consider the following sample CMakeLists.txt:

```cmake
project(myapp)

# This is how C++ will know about the zswag lib
# and its dependencies, such as zserio.
add_subdirectory(zswag)

add_zserio_module(<module-name>
    ROOT <zserio module source root>
    ENTRY <entrypoint zserio source. e.G. "main.zs">
    TOP_LEVEL_PKG <optional: top level package name>)
```

This will create two CMake targets `<module-name>` and `<module-name>-reflection`, the latter linking to the former.
To link against the generated zserio module, just link against `<module-name>-reflection`.

## Reflection

Zsr stores its top-level reflection metadata in a global list that can be accessed by calling `zsr::packages()`.
Reflection metadata types and their hierarchy can be found in [runtime/zsr/types.hpp](./runtime/zsr/types.hpp).

There are various other utility functions available. Check the respective source files for more information:
* [runtime/zsr/find.hpp](./runtime/zsr/find.hpp) – Functions for finding and resolving types
  * `zsr::find`: Find a reflection metadata object (Compound/Service Method/Field etc.) for a particular identifier.
* [runtime/zsr/getset.hpp](./runtime/zsr/getset.hpp) – Functions for getting and setting nested fields, and instantiating compounds.
  * `zsr::get`: Read a compound (nested) field value by name.
  * `zsr::set`: Set a compound (nested) field value by name.
  * `zsr::make`: Conveniently instantiate a compound with inline (nested) field value assignments. **Note:** Use `zsr::Compound::initialize` if your compound requires initialization parameters.
* [runtime/zsr/introspectable-json.hpp](./runtime/introspectable-json.hpp) – JSON export
  * `zsr::serialize`: Returns a JSON stream object from which a JSON string can be retrieved. **Note:** For standard binary serialization, use `zsr::Compound::write`.

Usage example:

```c++
#include "zsr/reflection-main.hpp"     /* Metadata root */
#include "zsr/find.hpp"                /* Find utilities */
#include "zsr/getset.hpp"              /* Get/set utilities */
#include "zsr/introspectable-json.hpp" /* JSON export */

...

/* Finding a compound named "Test" */
auto compound = zsr::find<zsr::Compound>(zsr::packages(), "Test");

/* Finding field "a" of compound "A" */
auto field = zsr::find<zsr::Field>(zsr::packages(), "A.a");

/* Instantiate compound "Test" */
auto instance = compound->alloc();

/* Set some field */
zsr::set(instance, "field_a", "My Value");

/* Convert to JSON */
auto json = zsr::serialize(instance).str();

```

## Examples

For more examples, have a look at the unit-tests under: [runtime/test/zserio](./runtime/test/zserio).
