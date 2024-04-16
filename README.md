# SwarmGame

Easy-to-setup and flexible environment to play swarm games.


## Getting Started

### Dependencies

Currently only working on Windows 10

* CMake
* MSVC with C++20 support

### Installing

* Clone this repo
```
git clone https://github.com/chiaxr/SwarmGame.git
```
* Download [libraries](https://drive.google.com/file/d/1r9HfjNzL4D28UYtx_wCTpORdUzKmw4uY/view) and unzip in cloned folder
* Generate solution
```
cd SwarmGame
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
```
* Build project in Visual Studio or MSBuild


## Acknowledgments

Third-party libraries used:
* [json](https://github.com/nlohmann/json)
* [raygui](https://github.com/raysan5/raygui)
* [raylib](https://github.com/raysan5/raylib)
* [ZeroMQ](https://zeromq.org/)
