# gtnh-2-packwiz
[![Build](https://github.com/p1x3l101-10/gtnh-2-packwiz/actions/workflows/build.yml/badge.svg)](https://github.com/p1x3l101-10/gtnh-2-packwiz/actions/workflows/build.yml) [![Valgrind leak checks](https://github.com/p1x3l101-10/gtnh-2-packwiz/actions/workflows/valgrind.yml/badge.svg)](https://github.com/p1x3l101-10/gtnh-2-packwiz/actions/workflows/valgrind.yml)

Not finished at all yet, but it is a proof of concept that GTNH can be distributed using packwiz

Although upstream uses DreamAssemblerXXL, that is written in python. I do not know python that well, but I will try to comment my code well enough so that it could be reimplimented if needed.

This code is licenced under BSD-3, basically that means that I really don't care what you do with it as long as I am credited (also you can't advertise a thing made with this using my name without approval (you can still advertise tho)). The binary has a flag that will spit out the license so that it abides by clause 2.

The list of things that are still needed to be done is in [`TODO.md`](./TODO.md)

The program is versioned using git tags following [semver](https://semver.org/)

Contributions are welcome, but I don't expect this to really take off...

You can view the list of needed dependancies to build in the [`CMakeLists.txt`](./CMakeLists.txt), or a nicer formatted version [here](./DEPENDANCIES.md)