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
* Download [libraries](https://drive.google.com/drive/folders/1XxWejEG9aGHbXzyk8PBnEdZnPmLPKYYv?usp=sharing) and unzip in cloned folder
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
