# HKSL

Hi there!

This is a simple shader language that I've been working on. It's a simple language with a rust like syntax, and it's designed to be easy to use and understand. It's not meant to be a full featured language. I started writing this language as I though it would be kinda cool to write a custom shader language to use with my game engine Hikari, hence the name HKSL (Hikari Shader Language).

Currently, there is a parser that can parse the language and generate an AST.
The plan is to write a full compiler that will atleast be able emit [SPIR-V](https://docs.vulkan.org/guide/latest/what_is_spirv.html) of simple RGB triangle shader in Vulkan.

Currently the plan is to use [MLIR](https://mlir.llvm.org/) as the intermediate representation, and then use the MLIR to generate the SPIR-V.

Even though Hikari is written in Rust, I've chosen to write the compiler in C++ as it's easier to work with MLIR in C++ currently. I might write a Rust version of the compiler in the future :)

### Example Syntax 

```rust
fn foo(x: vec3, y: vec3) -> vec3 {
    let z = x + y;
    return z;
}
fn fragment_main() {
    let color = foo(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    return color;
}
```
## Build

### Generate build
```bash
mkdir build && cd build
cmake -G Ninja ..
```

### Build
```bash
ninja
```

### Run
```bash
./hksl examples/playground.hksl
```