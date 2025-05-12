NOTE: sometimes `cURLpp`'s pkgconf file has a broken version field, you can build with the flag `-DBAD_CURLPP_VERSION_FORMAT` to get around this. THIS WILL NOT CHECK THE VERSIONS, YOU HAVE BEEN WARNED

The minimum versions here are the lib versions I developed and tested with, there are no guaruntees that this program works with older versions, but it might. I will not help you much if the versions are outdated though, I will just tell you to update the libs first unless this is definitavly a problem with my program (I will still tell you to update tho).

I also use LLVM clang++ to compile this, I *should'nt* have used any compiler specific things, but if a build breaks on another compiler (including apple clang) try using the LLVM clang++ to compile this and file a bug report saying that there is a compiler-ism used. If it still fails on clang++, then just file a normal bug

---
# Lib versions
- [Boost](https://www.boost.org/) 1.87.0
  - Components:
    - program_options
- [Magic Enum C++](https://github.com/Neargye/magic_enum) 0.9.5
- [Nlohmann JSON](https://github.com/nlohmann/json) 3.11.3
- [Toml++](https://marzer.github.io/tomlplusplus/) 3.4.0
- [Log 4 C++](https://log4cpp.sourceforge.net/) 1.1
- [cURLpp](https://www.curlpp.org/) 0.8.1

# Program versions (build time)
- [CMake](https://cmake.org) 3.31.6
- [PkgConf](http://pkgconf.org/) 1.8.1
- [LLVM](https://llvm.org) 20.1.3

# Testing (used for development) (not needed for client usage)
- [Valgrind](https://valgrind.org/), used to ensure memory safety